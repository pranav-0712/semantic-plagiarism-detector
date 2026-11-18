/*
 * ============================================================
 *  document_store.h  –  Linked List–Based Document Repository
 *  FDS Concept: Unit 5 – Linked Lists (Singly Linked List)
 *
 *  Every document loaded into the system is stored as a node
 *  in a singly linked list.  This mirrors the "Linked List as
 *  an ADT" concept from Unit 5 of the syllabus.
 * ============================================================
 */

#ifndef DOCUMENT_STORE_H
#define DOCUMENT_STORE_H

#include <string>
#include <vector>

/*
 * DocumentNode
 * ------------
 * A single node in the document linked list.
 * Holds the document's metadata and its pre-processed token array.
 */
struct DocumentNode {
    std::string              id;           // Unique identifier (e.g., "doc1")
    std::string              name;         // Human-readable filename / title
    std::string              rawContent;   // Original text as loaded from file
    std::vector<std::string> tokens;       // Normalized word tokens (array of strings)
    DocumentNode*            next;         // Pointer to the next document in the list

    // Constructor – initialises all fields and sets next to nullptr
    DocumentNode(const std::string& id,
                 const std::string& name,
                 const std::string& content);
};

/*
 * DocumentStore
 * -------------
 * A Singly Linked List ADT that acts as an in-memory database
 * for all reference documents loaded into the plagiarism detector.
 *
 * Supported operations:
 *  - addDocument      : O(n) – insert at tail to preserve insertion order
 *  - findById         : O(n) – linear search by ID
 *  - removeById       : O(n) – unlink and delete a node
 *  - displayAll       : O(n) – print all documents
 *  - getHead          : O(1) – returns the head pointer for iteration
 *  - getSize          : O(1) – returns current document count
 *  - loadFromFile     : reads a plain-text file and adds it as a document
 */
class DocumentStore {
private:
    DocumentNode* head;    // Pointer to the first node
    int           count;   // Running total of stored documents

public:
    // Constructor – initialises an empty list
    DocumentStore();

    // Destructor – frees every node to prevent memory leaks
    ~DocumentStore();

    /*
     * addDocument(id, name, content)
     * Tokenizes 'content', creates a new DocumentNode, and appends it
     * to the tail of the linked list.
     * Returns true on success, false if 'id' already exists.
     */
    bool addDocument(const std::string& id,
                     const std::string& name,
                     const std::string& content);

    /*
     * loadFromFile(filepath)
     * Reads the entire contents of 'filepath', strips the path to get
     * a human-readable name, and calls addDocument internally.
     * Returns true if the file was read and stored successfully.
     */
    bool loadFromFile(const std::string& filepath);

    /*
     * findById(id)
     * Performs a Linear Search through the linked list.
     * Returns a pointer to the matching DocumentNode, or nullptr if not found.
     * Time Complexity: O(n)
     */
    DocumentNode* findById(const std::string& id) const;

    /*
     * removeById(id)
     * Finds and unlinks the node with the given id, then frees its memory.
     * Returns true if the node was found and removed.
     * Time Complexity: O(n)
     */
    bool removeById(const std::string& id);

    /*
     * displayAll()
     * Traverses the linked list and prints a summary of every document.
     */
    void displayAll() const;

    // Accessors
    DocumentNode* getHead()  const { return head;  }
    int           getSize()  const { return count; }
    bool          isEmpty()  const { return head == nullptr; }
};

#endif // DOCUMENT_STORE_H
