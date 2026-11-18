/*
 * ============================================================
 *  document_store.cpp  –  Implementation of DocumentStore
 * ============================================================
 */

#include "document_store.h"
#include "string_matcher.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>   // C++17 for getting filename from path

// ============================================================
//  DocumentNode Constructor
// ============================================================
DocumentNode::DocumentNode(const std::string& id,
                           const std::string& name,
                           const std::string& content)
    : id(id), name(name), rawContent(content), next(nullptr)
{
    // Pre-process and tokenize the content when the node is created
    std::string normalised = StringMatcher::normalize(content);
    tokens = StringMatcher::tokenize(normalised);
}

// ============================================================
//  DocumentStore Constructor & Destructor
// ============================================================
DocumentStore::DocumentStore() : head(nullptr), count(0) {}

DocumentStore::~DocumentStore() {
    // Walk the list and delete every node to free heap memory
    DocumentNode* current = head;
    while (current != nullptr) {
        DocumentNode* next = current->next;
        delete current;
        current = next;
    }
    head  = nullptr;
    count = 0;
}

// ============================================================
//  addDocument
// ============================================================
bool DocumentStore::addDocument(const std::string& id,
                                const std::string& name,
                                const std::string& content) {
    // Reject duplicate IDs (linear search)
    if (findById(id) != nullptr) {
        std::cerr << "[DocumentStore] Warning: Document with id '" << id
                  << "' already exists. Skipping.\n";
        return false;
    }

    // Create the new node on the heap
    DocumentNode* newNode = new DocumentNode(id, name, content);

    // ---- Insert at TAIL to maintain insertion order ----
    if (head == nullptr) {
        // List is empty: new node becomes the head
        head = newNode;
    } else {
        // Traverse to the last node
        DocumentNode* current = head;
        while (current->next != nullptr)
            current = current->next;
        current->next = newNode;    // Link new node at the end
    }

    count++;
    return true;
}

// ============================================================
//  loadFromFile
// ============================================================
bool DocumentStore::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[DocumentStore] Error: Cannot open file '" << filepath << "'\n";
        return false;
    }

    // Read entire file content into a string
    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();

    if (content.empty()) {
        std::cerr << "[DocumentStore] Warning: File '" << filepath << "' is empty.\n";
        return false;
    }

    // Extract just the filename (without directory path) as the document name
    std::string filename = filepath;
    size_t lastSlash = filename.find_last_of("/\\");
    if (lastSlash != std::string::npos)
        filename = filename.substr(lastSlash + 1);

    // Use an auto-generated ID: "doc" + current count
    std::string id = "doc" + std::to_string(count + 1);

    std::cout << "[DocumentStore] Loaded: '" << filename
              << "' (id=" << id << ", "
              << content.size() << " chars)\n";

    return addDocument(id, filename, content);
}

// ============================================================
//  findById  –  Linear Search
// ============================================================
DocumentNode* DocumentStore::findById(const std::string& id) const {
    DocumentNode* current = head;
    // Traverse from head until we find the matching id or reach end
    while (current != nullptr) {
        if (current->id == id)
            return current;         // Found
        current = current->next;
    }
    return nullptr;                 // Not found
}

// ============================================================
//  removeById
// ============================================================
bool DocumentStore::removeById(const std::string& id) {
    if (head == nullptr) return false;

    // Special case: removing the head node
    if (head->id == id) {
        DocumentNode* toDelete = head;
        head = head->next;
        delete toDelete;
        count--;
        return true;
    }

    // General case: find the node BEFORE the one to remove
    DocumentNode* prev    = head;
    DocumentNode* current = head->next;

    while (current != nullptr) {
        if (current->id == id) {
            prev->next = current->next;   // Skip over the node
            delete current;
            count--;
            return true;
        }
        prev    = current;
        current = current->next;
    }
    return false;   // ID not found
}

// ============================================================
//  displayAll
// ============================================================
void DocumentStore::displayAll() const {
    if (head == nullptr) {
        std::cout << "  [Document Store is empty]\n";
        return;
    }

    std::cout << "\n  +-----------+---------------------------+--------+\n";
    std::cout <<   "  | ID        | Name                      | Tokens |\n";
    std::cout <<   "  +-----------+---------------------------+--------+\n";

    DocumentNode* current = head;
    int index = 1;
    while (current != nullptr) {
        // Format: index, id, name (truncated to 25 chars), token count
        std::string displayName = current->name;
        if (displayName.size() > 25) displayName = displayName.substr(0, 22) + "...";

        printf("  | %-9s | %-25s | %-6d |\n",
               current->id.c_str(),
               displayName.c_str(),
               static_cast<int>(current->tokens.size()));

        current = current->next;
        index++;
    }
    std::cout << "  +-----------+---------------------------+--------+\n";
    std::cout << "  Total documents in store: " << count << "\n\n";
}
