/*
 * ============================================================================
 *  Semantic Plagiarism Detector - single-file version
 * ============================================================================
 *
 *  Compares a suspect text against a set of reference documents and reports
 *  a similarity score for each one.
 *
 *  Pipeline
 *    1. Normalise   lowercase, replace punctuation with spaces
 *    2. Tokenise    split the text into an array of words
 *    3. Filter      remove stop-words using a Stack
 *    4. N-grams     build overlapping word sequences (default: 3 words)
 *    5. Match       find each suspect n-gram in the reference (naive matching)
 *    6. Score       Jaccard similarity = |A n B| / |A u B|
 *    7. Rank        merge sort, highest similarity first
 *
 *  Data structures
 *    - Array-based generic Stack ......... stop-word filtering
 *    - Singly linked list ................ reference document store
 *    - Arrays and strings ................ tokens and n-grams
 *    - Naive string matching ............. n-gram lookup
 *    - Merge sort ........................ ranking results
 *    - Linear search ..................... finding documents by ID
 *
 *  Build and run (VS Code: Terminal > New Terminal)
 *    g++ -std=c++17 -Wall plagiarism_demo.cpp -o plagiarism_demo
 *    .\plagiarism_demo.exe        (Windows)
 *    ./plagiarism_demo            (Linux / macOS)
 *
 *  The program runs a complete demonstration first, then opens an
 *  interactive menu for checking your own text.
 * ============================================================================
 */

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <stdexcept>

// ----------------------------------------------------------------------------
//  Configuration
// ----------------------------------------------------------------------------
const double THRESHOLD_HIGH   = 0.25;   // >= 25%  -> HIGH PLAGIARISM
const double THRESHOLD_MEDIUM = 0.10;   // >= 10%  -> MODERATE PLAGIARISM
const int    DEFAULT_NGRAM    = 3;      // trigrams

// Common English words that carry little meaning on their own.
const char* STOP_WORDS[] = {
    "a","an","the","and","or","but","in","on","at","to","for","of","with",
    "is","are","was","were","be","been","being","have","has","had","do",
    "does","did","will","would","could","should","may","might","shall",
    "not","no","nor","so","yet","both","either","neither","than","as",
    "if","while","although","because","since","unless","until","after",
    "before","between","above","below","by","from","up","down","about",
    "into","through","during","it","its","this","that","these","those",
    "i","you","he","she","we","they","me","him","her","us","them","my",
    "your","his","our","their","what","which","who","whom","how","when",
    "where","why","all","any","each","every","few","more","most","other",
    "some","such","only","own","same","just","very","can","also","there"
};
const int STOP_WORD_COUNT = sizeof(STOP_WORDS) / sizeof(STOP_WORDS[0]);


// ============================================================================
//  1. STACK  (generic, array-based, grows when full)
// ============================================================================
template <typename T>
class Stack {
private:
    T*  data;
    int capacity;
    int topIndex;           // -1 means the stack is empty

    void grow() {
        int newCapacity = capacity * 2;
        T* newData = new T[newCapacity];
        for (int i = 0; i <= topIndex; i++)
            newData[i] = data[i];
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

public:
    explicit Stack(int initialCapacity = 64)
        : data(new T[initialCapacity]), capacity(initialCapacity), topIndex(-1) {}

    ~Stack() { delete[] data; }

    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;

    void push(const T& value) {                 // O(1) amortised
        if (topIndex + 1 == capacity) grow();
        data[++topIndex] = value;
    }

    T pop() {                                   // O(1)
        if (isEmpty()) throw std::underflow_error("Stack underflow");
        return data[topIndex--];
    }

    const T& peek() const {                     // O(1)
        if (isEmpty()) throw std::underflow_error("Stack is empty");
        return data[topIndex];
    }

    bool isEmpty() const { return topIndex == -1; }
    int  size()    const { return topIndex + 1; }
};


// ============================================================================
//  2. STRING PROCESSING  (arrays, strings, naive string matching)
// ============================================================================

// Lowercase letters/digits are kept; everything else becomes a space.  O(n)
std::string normalise(const std::string& text) {
    std::string out;
    out.reserve(text.size());
    for (size_t i = 0; i < text.size(); i++) {
        unsigned char c = static_cast<unsigned char>(text[i]);
        out += std::isalnum(c) ? static_cast<char>(std::tolower(c)) : ' ';
    }
    return out;
}

// Splits on whitespace into an array of words.  O(n)
std::vector<std::string> tokenise(const std::string& text) {
    std::vector<std::string> tokens;
    std::istringstream stream(text);
    std::string word;
    while (stream >> word)
        tokens.push_back(word);
    return tokens;
}

// Linear search through the stop-word array.
bool isStopWord(const std::string& word) {
    for (int i = 0; i < STOP_WORD_COUNT; i++)
        if (word == STOP_WORDS[i]) return true;
    return false;
}

// Uses a Stack to keep only meaningful words (longer than 2 letters and not
// stop-words). Popping reverses the order, so the result is reversed back.
std::vector<std::string> filterStopWords(const std::vector<std::string>& tokens) {
    Stack<std::string> stack;
    for (size_t i = 0; i < tokens.size(); i++)
        if (tokens[i].size() > 2 && !isStopWord(tokens[i]))
            stack.push(tokens[i]);

    std::vector<std::string> reversed;
    while (!stack.isEmpty())
        reversed.push_back(stack.pop());

    std::vector<std::string> meaningful;
    for (int i = static_cast<int>(reversed.size()) - 1; i >= 0; i--)
        meaningful.push_back(reversed[i]);
    return meaningful;
}

// Sliding window of n consecutive words.  O(t * n)
std::vector<std::string> generateNGrams(const std::vector<std::string>& tokens, int n) {
    std::vector<std::string> ngrams;
    for (int i = 0; i + n <= static_cast<int>(tokens.size()); i++) {
        std::string gram = tokens[i];
        for (int j = 1; j < n; j++)
            gram += " " + tokens[i + j];
        ngrams.push_back(gram);
    }
    return ngrams;
}

// Naive (brute-force) string matching. Returns the first index or -1.  O(n * m)
int naiveSearch(const std::string& text, const std::string& pattern) {
    int n = static_cast<int>(text.size());
    int m = static_cast<int>(pattern.size());
    if (m == 0) return 0;
    for (int i = 0; i <= n - m; i++) {
        int j = 0;
        while (j < m && text[i + j] == pattern[j])
            j++;
        if (j == m) return i;
    }
    return -1;
}


// ============================================================================
//  3. LINKED LIST  (reference document store)
// ============================================================================
struct DocumentNode {
    std::string   id;
    std::string   name;
    std::string   content;
    DocumentNode* next;

    DocumentNode(const std::string& i, const std::string& nm, const std::string& c)
        : id(i), name(nm), content(c), next(nullptr) {}
};

class DocumentStore {
private:
    DocumentNode* head;
    int           count;

public:
    DocumentStore() : head(nullptr), count(0) {}

    ~DocumentStore() {
        while (head != nullptr) {
            DocumentNode* temp = head;
            head = head->next;
            delete temp;
        }
    }

    DocumentStore(const DocumentStore&) = delete;
    DocumentStore& operator=(const DocumentStore&) = delete;

    // Linear search by ID.  O(n)
    DocumentNode* findById(const std::string& id) const {
        DocumentNode* current = head;
        while (current != nullptr) {
            if (current->id == id) return current;
            current = current->next;
        }
        return nullptr;
    }

    // Append at the tail (keeps insertion order). Rejects duplicate IDs.  O(n)
    bool add(const std::string& id, const std::string& name, const std::string& content) {
        if (findById(id) != nullptr) return false;
        DocumentNode* node = new DocumentNode(id, name, content);
        if (head == nullptr) {
            head = node;
        } else {
            DocumentNode* current = head;
            while (current->next != nullptr)
                current = current->next;
            current->next = node;
        }
        count++;
        return true;
    }

    // Unlink and delete the node with the given ID.  O(n)
    bool remove(const std::string& id) {
        DocumentNode* current  = head;
        DocumentNode* previous = nullptr;
        while (current != nullptr && current->id != id) {
            previous = current;
            current  = current->next;
        }
        if (current == nullptr) return false;
        if (previous == nullptr) head = current->next;
        else                     previous->next = current->next;
        delete current;
        count--;
        return true;
    }

    DocumentNode* getHead() const { return head; }
    int  size()    const { return count; }
    bool isEmpty() const { return count == 0; }

    void print() const {
        if (isEmpty()) { std::cout << "  (no reference documents)\n"; return; }
        std::cout << "  " << std::left << std::setw(8) << "ID"
                  << std::setw(34) << "Name" << "Words\n";
        std::cout << "  " << std::string(48, '-') << "\n";
        for (DocumentNode* cur = head; cur != nullptr; cur = cur->next)
            std::cout << "  " << std::left << std::setw(8) << cur->id
                      << std::setw(34) << cur->name.substr(0, 32)
                      << tokenise(normalise(cur->content)).size() << "\n";
        std::cout << "  Total: " << count << " document(s) in the linked list\n";
    }
};


// ============================================================================
//  4. RESULTS AND MERGE SORT
// ============================================================================
struct Result {
    std::string id;
    std::string name;
    double      similarity;        // 0.0 to 1.0
    int         matched;           // |A n B|
    int         suspectNGrams;     // |B|
    int         referenceNGrams;   // |A|
    std::vector<std::string> phrases;
};

void merge(std::vector<Result>& arr, int left, int mid, int right) {
    std::vector<Result> L(arr.begin() + left, arr.begin() + mid + 1);
    std::vector<Result> R(arr.begin() + mid + 1, arr.begin() + right + 1);
    size_t i = 0, j = 0;
    int k = left;
    while (i < L.size() && j < R.size()) {
        if (L[i].similarity >= R[j].similarity) arr[k++] = L[i++];   // descending
        else                                     arr[k++] = R[j++];
    }
    while (i < L.size()) arr[k++] = L[i++];
    while (j < R.size()) arr[k++] = R[j++];
}

// O(n log n), stable.
void mergeSort(std::vector<Result>& arr, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}


// ============================================================================
//  5. DETECTOR
// ============================================================================
std::string verdict(double similarity) {
    if (similarity >= THRESHOLD_HIGH)   return "HIGH PLAGIARISM";
    if (similarity >= THRESHOLD_MEDIUM) return "MODERATE PLAGIARISM";
    if (similarity > 0.0)               return "LOW SIMILARITY";
    return "ORIGINAL";
}

std::string percent(double value) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << value * 100.0 << "%";
    return out.str();
}

// Joins the first `limit` items of a list for display.
std::string preview(const std::vector<std::string>& items, size_t limit, const char* sep) {
    std::string out;
    for (size_t i = 0; i < items.size() && i < limit; i++) {
        if (i > 0) out += sep;
        out += items[i];
    }
    if (items.size() > limit) out += std::string(sep) + "...";
    return out;
}

// Steps 1-4 for one text: normalise, tokenise, filter, n-grams.
std::vector<std::string> textToNGrams(const std::string& text, int n) {
    return generateNGrams(filterStopWords(tokenise(normalise(text))), n);
}

// Jaccard similarity using naive matching.
// Reference n-grams are joined with '|' separators so a match can only
// occur on a whole n-gram, never across two of them.
double jaccard(const std::vector<std::string>& reference,
               const std::vector<std::string>& suspect,
               int& matched, std::vector<std::string>& phrases) {
    matched = 0;
    if (reference.empty() || suspect.empty()) return 0.0;

    std::string blob;
    for (size_t i = 0; i < reference.size(); i++)
        blob += "|" + reference[i] + "|";

    for (size_t i = 0; i < suspect.size(); i++) {
        if (naiveSearch(blob, "|" + suspect[i] + "|") != -1) {
            matched++;
            bool seen = false;                          // keep phrases unique
            for (size_t k = 0; k < phrases.size() && !seen; k++)
                seen = (phrases[k] == suspect[i]);
            if (!seen) phrases.push_back(suspect[i]);
        }
    }
    double unionSize = static_cast<double>(reference.size() + suspect.size() - matched);
    return unionSize > 0 ? matched / unionSize : 0.0;
}

// Full analysis. With verbose = true every pipeline step is printed.
std::vector<Result> analyse(const std::string& suspectText, const DocumentStore& store,
                            int n, bool verbose) {
    std::vector<Result> results;

    std::string              normalised = normalise(suspectText);
    std::vector<std::string> tokens     = tokenise(normalised);
    std::vector<std::string> filtered   = filterStopWords(tokens);
    std::vector<std::string> suspect    = generateNGrams(filtered, n);

    if (verbose) {
        std::cout << "\n  [Step 1] Normalise\n"
                  << "           \"" << normalised.substr(0, 70) << (normalised.size() > 70 ? "..." : "") << "\"\n";
        std::cout << "  [Step 2] Tokenise -> " << tokens.size() << " words\n"
                  << "           " << preview(tokens, 10, ", ") << "\n";
        std::cout << "  [Step 3] Stop-word filter using Stack -> kept " << filtered.size()
                  << " of " << tokens.size() << " words\n"
                  << "           " << preview(filtered, 10, ", ") << "\n";
        std::cout << "  [Step 4] Generate " << n << "-grams -> " << suspect.size() << " n-grams\n"
                  << "           " << preview(suspect, 4, " | ") << "\n";
    }

    if (suspect.empty()) {
        std::cout << "\n  The text is too short. It needs at least " << n
                  << " meaningful words after filtering.\n";
        return results;
    }

    if (verbose)
        std::cout << "  [Step 5] Compare with each document (linked list traversal + naive matching)\n";

    for (DocumentNode* doc = store.getHead(); doc != nullptr; doc = doc->next) {
        Result r;
        r.id   = doc->id;
        r.name = doc->name;
        std::vector<std::string> reference = textToNGrams(doc->content, n);
        r.similarity      = jaccard(reference, suspect, r.matched, r.phrases);
        r.suspectNGrams   = static_cast<int>(suspect.size());
        r.referenceNGrams = static_cast<int>(reference.size());
        results.push_back(r);

        if (verbose) {
            int unionSize = r.referenceNGrams + r.suspectNGrams - r.matched;
            std::cout << "           " << std::left << std::setw(30) << r.name.substr(0, 29)
                      << "J = " << r.matched << " / " << unionSize
                      << " = " << percent(r.similarity) << "\n";
        }
    }

    if (!results.empty())
        mergeSort(results, 0, static_cast<int>(results.size()) - 1);

    if (verbose)
        std::cout << "  [Step 6] Rank results using merge sort (highest first)\n";

    return results;
}

void printReport(const std::vector<Result>& results) {
    if (results.empty()) return;

    std::string line = "  +------+--------------------------------+------------+---------------------+";
    std::cout << "\n" << line << "\n";
    std::cout << "  | Rank | Reference document             | Similarity | Verdict             |\n";
    std::cout << line << "\n";
    for (size_t i = 0; i < results.size(); i++) {
        std::cout << "  | " << std::left << std::setw(5) << (i + 1)
                  << "| " << std::setw(31) << results[i].name.substr(0, 30)
                  << "| " << std::right << std::setw(10) << percent(results[i].similarity) << " "
                  << "| " << std::left << std::setw(20) << verdict(results[i].similarity) << "|\n";
    }
    std::cout << line << "\n";

    const Result& top = results[0];
    std::cout << "\n  Verdict: " << verdict(top.similarity);
    if (top.similarity > 0.0) {
        std::cout << " (closest match: " << top.name << ")\n";
        std::cout << "  " << top.matched << " of " << top.suspectNGrams
                  << " suspect n-grams were found in this document.\n";
        std::cout << "  Matching phrases (" << top.phrases.size() << " unique):\n";
        for (size_t i = 0; i < top.phrases.size() && i < 8; i++)
            std::cout << "    - " << top.phrases[i] << "\n";
        if (top.phrases.size() > 8)
            std::cout << "    ... and " << (top.phrases.size() - 8) << " more\n";
    } else {
        std::cout << " (no shared n-grams with any reference document)\n";
    }
}


// ============================================================================
//  6. SAMPLE DATA
// ============================================================================
const char* REF_ML =
    "Machine learning is a branch of artificial intelligence that enables "
    "systems to learn from data and improve their performance over time "
    "without being explicitly programmed. It uses algorithms to parse data, "
    "learn from it, and make informed decisions. Machine learning is "
    "applied in many fields including computer vision, natural language "
    "processing, and speech recognition. Deep learning is a subset of "
    "machine learning that uses neural networks with multiple layers to "
    "model complex patterns in data. These deep neural networks are "
    "inspired by the structure and function of the human brain. Deep "
    "learning has achieved remarkable results in image recognition, "
    "language translation, and autonomous driving systems. Supervised "
    "learning is a type of machine learning where the model is trained on "
    "labelled data. The algorithm learns to map inputs to outputs based on "
    "example input-output pairs. Classification and regression are two key "
    "tasks performed using supervised learning algorithms. Unsupervised "
    "learning, on the other hand, finds hidden patterns in data without "
    "labelled responses. Clustering, dimensionality reduction, and "
    "generative modelling are common unsupervised learning techniques. The "
    "k-means clustering algorithm is a popular unsupervised learning "
    "method. Reinforcement learning is a paradigm where an agent learns to "
    "make decisions by interacting with an environment to maximise a "
    "cumulative reward signal. This approach has led to breakthroughs in "
    "game playing, robotics, and resource management systems.";

const char* REF_DS =
    "Data structures are specialised formats for organising, processing, "
    "retrieving and storing data. They make it easier to manipulate data "
    "efficiently. Common data structures include arrays, linked lists, "
    "stacks, queues, trees and graphs. Choosing the right data structure "
    "for a problem is key to writing efficient algorithms. An array is a "
    "collection of elements stored at contiguous memory locations. It is "
    "the simplest and most widely used data structure. Arrays support "
    "random access in O(1) time but insertions and deletions take O(n) "
    "time. Static arrays have a fixed size, while dynamic arrays can resize "
    "themselves at runtime. A linked list is a linear data structure where "
    "each element, called a node, contains data and a pointer to the next "
    "node. Unlike arrays, linked lists do not require contiguous memory. "
    "Insertion and deletion at the head of a linked list take O(1) time. "
    "Traversal from the head to any node takes O(n) time. A stack is a "
    "linear data structure that follows the Last-In-First-Out (LIFO) "
    "principle. The push operation adds an element to the top, and the pop "
    "operation removes the top element. Stacks are used in function call "
    "management, expression evaluation, and backtracking algorithms. A "
    "queue is a linear data structure that follows the First-In-First-Out "
    "(FIFO) principle. Elements are added at the rear and removed from the "
    "front. Queues are widely used in scheduling, breadth-first search, and "
    "buffering applications. Sorting algorithms arrange elements of a list "
    "in a specific order. Common sorting algorithms include bubble sort, "
    "insertion sort, selection sort, merge sort, quick sort, and heap sort. "
    "Merge sort and quick sort are efficient divide-and-conquer algorithms "
    "with average time complexity of O(n log n).";

const char* REF_IR =
    "The Industrial Revolution began in Britain in the late eighteenth "
    "century and transformed the way goods were produced. Before this "
    "period, most manufacturing was done by hand in small workshops or "
    "homes. The invention of the steam engine allowed factories to operate "
    "machinery on a large scale, which greatly increased productivity. "
    "Workers moved from rural villages to growing cities in search of "
    "employment, leading to rapid urbanisation. Working conditions in early "
    "factories were often harsh, with long hours, low wages and dangerous "
    "machinery. Over time, these conditions led to the formation of trade "
    "unions and the introduction of labour laws that protected workers' "
    "rights.";

const char* SUSPECT_COPIED =
    "Machine learning is a branch of artificial intelligence that enables "
    "systems to learn from data and improve their performance over time "
    "without being explicitly programmed. It uses algorithms to parse data "
    "and make informed decisions. Machine learning is applied in fields "
    "like computer vision and natural language processing. Deep learning, a "
    "subset of machine learning, uses neural networks with many layers to "
    "model complex patterns. These deep neural networks are inspired by the "
    "structure of the human brain, and deep learning has achieved "
    "remarkable results in image recognition and language translation. This "
    "technology has completely transformed modern computing and continues "
    "to shape the future of automation and intelligent systems across all "
    "industries.";

const char* SUSPECT_PARTIAL =
    "The Industrial Revolution was one of the most important turning points "
    "in history. It began in Britain and spread across Europe and North "
    "America during the nineteenth century. The invention of the steam "
    "engine allowed factories to operate machinery on a large scale, which "
    "greatly increased productivity. New industries such as textiles, coal "
    "mining and iron making grew quickly, and railways made it far easier "
    "to move raw materials and finished products. Working conditions in "
    "early factories were often harsh, with long hours, low wages and "
    "dangerous machinery. These changes also created new social classes and "
    "reshaped family life.";

const char* SUSPECT_ORIGINAL =
    "Between roughly 1760 and 1840, Britain shifted from an economy based "
    "on farming and handicrafts to one driven by machines. Steam power made "
    "it possible to run mills and mines far more efficiently than before. "
    "As jobs appeared in towns, people left the countryside, and cities "
    "such as Manchester expanded very quickly. Life in these new industrial "
    "towns was difficult, and campaigns for fairer treatment eventually "
    "changed the law.";

void loadSampleReferences(DocumentStore& store) {
    store.add("doc1", "Machine Learning Overview", REF_ML);
    store.add("doc2", "Data Structures Introduction", REF_DS);
    store.add("doc3", "Industrial Revolution", REF_IR);
}


// ============================================================================
//  7. USER INTERFACE
// ============================================================================
void printBanner() {
    std::cout << "\n  ==========================================================================\n";
    std::cout << "                      SEMANTIC PLAGIARISM DETECTOR\n";
    std::cout << "         N-gram analysis  |  Jaccard similarity  |  Merge sort ranking\n";
    std::cout << "  ==========================================================================\n";
}

void printHeading(const std::string& title) {
    std::cout << "\n  --------------------------------------------------------------------------\n";
    std::cout << "   " << title << "\n";
    std::cout << "  --------------------------------------------------------------------------\n";
}

void runDemo(int n) {
    DocumentStore store;
    loadSampleReferences(store);

    printHeading("REFERENCE DOCUMENTS (stored in a singly linked list)");
    store.print();

    printHeading("TEST 1 of 3: Text copied from a reference with small edits");
    std::cout << "  Suspect: \"" << std::string(SUSPECT_COPIED).substr(0, 66) << "...\"\n";
    printReport(analyse(SUSPECT_COPIED, store, n, true));

    printHeading("TEST 2 of 3: Original text with two copied sentences");
    std::cout << "  Suspect: \"" << std::string(SUSPECT_PARTIAL).substr(0, 66) << "...\"\n";
    printReport(analyse(SUSPECT_PARTIAL, store, n, false));

    printHeading("TEST 3 of 3: Same topic, written in different words");
    std::cout << "  Suspect: \"" << std::string(SUSPECT_ORIGINAL).substr(0, 66) << "...\"\n";
    printReport(analyse(SUSPECT_ORIGINAL, store, n, false));
}

// Reads one line. Returns false when no more input is available.
bool readLine(const std::string& prompt, std::string& out) {
    std::cout << prompt << std::flush;
    if (!std::getline(std::cin, out)) return false;
    if (!out.empty() && out[out.size() - 1] == '\r') out.erase(out.size() - 1);   // Windows line endings
    return true;
}

// Reads lines until an empty line is entered.
bool readParagraph(const std::string& prompt, std::string& out) {
    std::cout << prompt << "\n  (press Enter on an empty line to finish)\n";
    out.clear();
    std::string line;
    while (readLine("  > ", line)) {
        if (line.empty()) return true;
        out += line + " ";
    }
    return !out.empty();
}

void printMenu(int n) {
    std::cout << "\n  ------------------------- MAIN MENU -------------------------\n";
    std::cout << "   1. Check text for plagiarism\n";
    std::cout << "   2. Add a reference document\n";
    std::cout << "   3. List reference documents\n";
    std::cout << "   4. Remove a reference document\n";
    std::cout << "   5. Change n-gram size (current: " << n << ")\n";
    std::cout << "   6. Run the demonstration again\n";
    std::cout << "   0. Exit\n";
    std::cout << "  -------------------------------------------------------------\n";
}

int main() {
    int n = DEFAULT_NGRAM;

    printBanner();
    runDemo(n);

    // Interactive mode starts with the three sample references loaded.
    DocumentStore store;
    loadSampleReferences(store);
    int nextId = 4;

    while (true) {
        printMenu(n);
        std::string choice;
        if (!readLine("  Enter choice: ", choice)) {
            std::cout << "\n\n  Demonstration complete. To use the menu, run the program in a terminal.\n\n";
            return 0;
        }

        if (choice == "1") {
            if (store.isEmpty()) { std::cout << "  Add a reference document first.\n"; continue; }
            std::string text;
            if (!readParagraph("\n  Enter the text to check:", text) || text.empty()) continue;
            printReport(analyse(text, store, n, true));

        } else if (choice == "2") {
            std::string name, text;
            if (!readLine("  Document name: ", name)) break;
            if (name.empty()) name = "Document " + std::to_string(nextId);
            if (!readParagraph("  Enter the document text:", text) || text.empty()) continue;
            std::string id = "doc" + std::to_string(nextId++);
            store.add(id, name, text);
            std::cout << "  Added \"" << name << "\" with ID " << id << ".\n";

        } else if (choice == "3") {
            store.print();

        } else if (choice == "4") {
            store.print();
            std::string id;
            if (!readLine("  ID to remove: ", id)) break;
            DocumentNode* doc = store.findById(id);
            if (doc == nullptr) {
                std::cout << "  No document with ID \"" << id << "\".\n";
            } else {
                std::string name = doc->name;
                store.remove(id);
                std::cout << "  Removed \"" << name << "\".\n";
            }

        } else if (choice == "5") {
            std::string value;
            if (!readLine("  New n-gram size (2-5): ", value)) break;
            if (value.size() == 1 && value[0] >= '2' && value[0] <= '5') {
                n = value[0] - '0';
                std::cout << "  N-gram size set to " << n << ".\n";
            } else {
                std::cout << "  Please enter a number from 2 to 5.\n";
            }

        } else if (choice == "6") {
            runDemo(n);

        } else if (choice == "0") {
            std::cout << "\n  Thank you for using Semantic Plagiarism Detector.\n\n";
            return 0;

        } else {
            std::cout << "  Invalid choice. Enter a number from 0 to 6.\n";
        }
    }
    std::cout << "\n";
    return 0;
}
