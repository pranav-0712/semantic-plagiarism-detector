/*
 * ============================================================
 *  plagiarism_detector.cpp  –  Core Detection Engine Implementation
 *  FDS Concepts: ADT, Arrays, Strings, Stack, Linked List, Sorting
 * ============================================================
 */

#include "plagiarism_detector.h"
#include "string_matcher.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <unordered_set>
#include <fstream>
#include <sstream>

// ---- Common English stop-words to filter out (improves precision) ----
static const std::unordered_set<std::string> STOP_WORDS = {
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

// ============================================================
//  Constructor & Destructor
// ============================================================
PlagiarismDetector::PlagiarismDetector(int ngramSize)
    : ngramSize(ngramSize)
{
    store = new DocumentStore();
}

PlagiarismDetector::~PlagiarismDetector() {
    delete store;
}

// ============================================================
//  processTokensWithStack
//  FDS Concept: Unit 4 – Stack ADT
//  Uses a Stack to push tokens one-by-one and pop only the
//  ones that are NOT stop-words, effectively filtering the stream.
// ============================================================
std::vector<std::string> PlagiarismDetector::processTokensWithStack(
        const std::vector<std::string>& tokens) const {

    Stack<std::string> tokenStack;
    std::vector<std::string> meaningful;

    // Push every token onto the stack
    for (const std::string& token : tokens) {
        // Only push tokens that are:
        //  (a) not in the stop-word list, and
        //  (b) longer than 2 characters (single-letter or 2-letter words add noise)
        if (token.size() > 2 && STOP_WORDS.find(token) == STOP_WORDS.end()) {
            tokenStack.push(token);
        }
    }

    // Pop all items off the stack into a temporary vector (LIFO → reversed order)
    std::vector<std::string> reversed;
    while (!tokenStack.isEmpty()) {
        reversed.push_back(tokenStack.pop());
    }

    // Reverse again to restore the original order
    for (int i = static_cast<int>(reversed.size()) - 1; i >= 0; i--)
        meaningful.push_back(reversed[i]);

    return meaningful;
}

// ============================================================
//  calculateJaccard
//  Jaccard Similarity = |A ∩ B| / |A ∪ B|
// ============================================================
double PlagiarismDetector::calculateJaccard(
        const std::vector<std::string>& setA,
        const std::vector<std::string>& setB,
        std::vector<std::string>&       common) const {

    if (setA.empty() || setB.empty()) return 0.0;

    // Count matched n-grams using Naive String Matching
    int matchCount = StringMatcher::countNGramMatches(setA, setB, common);

    // |A ∪ B| = |A| + |B| - |A ∩ B|   (inclusion-exclusion principle)
    double unionSize = static_cast<double>(setA.size() + setB.size() - matchCount);

    if (unionSize <= 0.0) return 0.0;

    return static_cast<double>(matchCount) / unionSize;
}

// ============================================================
//  loadReferenceFile
// ============================================================
bool PlagiarismDetector::loadReferenceFile(const std::string& filepath) {
    return store->loadFromFile(filepath);
}

// ============================================================
//  addReferenceText
// ============================================================
bool PlagiarismDetector::addReferenceText(const std::string& id,
                                          const std::string& name,
                                          const std::string& text) {
    return store->addDocument(id, name, text);
}

// ============================================================
//  listDocuments
// ============================================================
void PlagiarismDetector::listDocuments() const {
    store->displayAll();
}

// ============================================================
//  removeDocument
// ============================================================
bool PlagiarismDetector::removeDocument(const std::string& id) {
    return store->removeById(id);
}

// ============================================================
//  analyse  –  Main plagiarism detection pipeline
// ============================================================
std::vector<SimilarityResult> PlagiarismDetector::analyse(
        const std::string& suspectText) const {

    std::vector<SimilarityResult> results;

    if (store->isEmpty()) {
        std::cout << "[Detector] No reference documents loaded. Please add documents first.\n";
        return results;
    }

    // ---- Step 1: Pre-process the suspect text ----
    std::string normSuspect = StringMatcher::normalize(suspectText);
    std::vector<std::string> suspectTokens = StringMatcher::tokenize(normSuspect);

    // ---- Step 2: Filter stop-words using the Stack (Unit 4) ----
    std::vector<std::string> filteredSuspect = processTokensWithStack(suspectTokens);

    // ---- Step 3: Generate n-grams for the suspect text (Unit 2 – Strings) ----
    std::vector<std::string> suspectNGrams = StringMatcher::generateNGrams(filteredSuspect, ngramSize);

    if (suspectNGrams.empty()) {
        std::cout << "[Detector] Warning: Suspect text too short to generate "
                  << ngramSize << "-grams.\n";
        return results;
    }

    // ---- Step 4: Compare against every reference document (Unit 5 – Linked List traversal) ----
    DocumentNode* current = store->getHead();
    while (current != nullptr) {

        // Filter stop-words from reference document tokens
        std::vector<std::string> filteredRef = processTokensWithStack(current->tokens);

        // Generate n-grams for this reference document
        std::vector<std::string> refNGrams = StringMatcher::generateNGrams(filteredRef, ngramSize);

        if (!refNGrams.empty()) {
            // ---- Step 5: Compute Jaccard Similarity (Naive String Matching inside) ----
            SimilarityResult result;
            result.docId       = current->id;
            result.docName     = current->name;
            result.totalSuspect = static_cast<int>(suspectNGrams.size());
            result.similarity  = calculateJaccard(refNGrams, suspectNGrams, result.matchedPhrases);
            result.matchedCount = static_cast<int>(result.matchedPhrases.size());
            results.push_back(result);
        }

        current = current->next;    // Move to next linked list node
    }

    // ---- Step 6: Sort results by descending similarity (Merge Sort – Unit 3) ----
    if (!results.empty())
        Sorter::mergeSort(results, 0, static_cast<int>(results.size()) - 1);

    return results;
}

// ============================================================
//  getVerdict
// ============================================================
std::string PlagiarismDetector::getVerdict(double similarity) {
    if (similarity >= THRESHOLD_HIGH)
        return "HIGH PLAGIARISM";
    else if (similarity >= THRESHOLD_MEDIUM)
        return "MODERATE PLAGIARISM";
    else if (similarity > 0.0)
        return "LOW SIMILARITY";
    else
        return "ORIGINAL";
}

// ============================================================
//  printReport
// ============================================================
void PlagiarismDetector::printReport(const std::vector<SimilarityResult>& results,
                                     const std::string& suspectName) {
    std::cout << "\n";
    std::cout << "  ============================================================\n";
    std::cout << "   PLAGIARISM ANALYSIS REPORT\n";
    std::cout << "   Suspect Document : " << suspectName << "\n";
    std::cout << "  ============================================================\n\n";

    if (results.empty()) {
        std::cout << "  No results to display.\n";
        return;
    }

    // Header
    std::cout << "  Rank | Document                   | Similarity | Verdict\n";
    std::cout << "  -----+----------------------------+------------+-------------------\n";

    for (int i = 0; i < (int)results.size(); i++) {
        const SimilarityResult& r = results[i];
        double pct = r.similarity * 100.0;

        std::string docDisplay = r.docName;
        if (docDisplay.size() > 26) docDisplay = docDisplay.substr(0, 23) + "...";

        std::string verdict = getVerdict(r.similarity);

        printf("  %-4d | %-26s | %8.2f%%  | %s\n",
               i + 1,
               docDisplay.c_str(),
               pct,
               verdict.c_str());
    }

    // ---- Detailed report for the top match ----
    const SimilarityResult& top = results[0];
    if (top.similarity > 0.0) {
        std::cout << "\n  ---- Top Match Details (" << top.docName << ") ----\n";
        std::cout << "  Matched " << top.matchedCount << " out of "
                  << top.totalSuspect << " n-grams ("
                  << std::fixed << std::setprecision(1)
                  << (top.similarity * 100.0) << "% similarity)\n";

        if (!top.matchedPhrases.empty()) {
            std::cout << "\n  Common Phrases Found:\n";
            int show = std::min(10, static_cast<int>(top.matchedPhrases.size()));
            for (int i = 0; i < show; i++)
                std::cout << "    [" << (i + 1) << "] \"" << top.matchedPhrases[i] << "\"\n";
            if ((int)top.matchedPhrases.size() > 10)
                std::cout << "    ... and " << (top.matchedPhrases.size() - 10) << " more.\n";
        }
    }

    std::cout << "\n  ============================================================\n";
    std::cout << "   Overall Verdict: " << getVerdict(top.similarity) << "\n";
    std::cout << "  ============================================================\n\n";
}
