/*
 * ============================================================
 *  plagiarism_detector.h  –  Core Detection Engine
 *  FDS Concept: ADT (Unit 1), integrates all other modules
 *
 *  PlagiarismDetector is the central class that ties together:
 *   - DocumentStore  (linked list of reference docs)
 *   - StringMatcher  (tokenisation + naive string matching)
 *   - Sorter         (merge sort for ranking results)
 *   - Stack          (token-processing pipeline)
 * ============================================================
 */

#ifndef PLAGIARISM_DETECTOR_H
#define PLAGIARISM_DETECTOR_H

#include "document_store.h"
#include "sorter.h"
#include "stack.h"

#include <string>
#include <vector>

// Threshold constants for verdict classification
// Note: Scores are lower than expected because stop-word filtering removes
// common words, leaving only content words in the n-gram pool.
// These thresholds are calibrated for content-word-only Jaccard similarity.
constexpr double THRESHOLD_HIGH   = 0.25;   // >= 25% → High plagiarism
constexpr double THRESHOLD_MEDIUM = 0.10;   // >= 10% → Moderate plagiarism
// < 10% → Low / No significant plagiarism

/*
 * PlagiarismDetector
 * ------------------
 * Acts as a Facade (wrapper) over the other modules.
 * The user interacts only with this class.
 */
class PlagiarismDetector {
private:
    DocumentStore* store;       // Linked list of reference documents
    int            ngramSize;   // Size of n-grams used for comparison (default=3)

    /*
     * processTokensWithStack(tokens)
     * Uses a Stack<std::string> to filter stop-words and process
     * the token stream before n-gram generation.
     * Demonstrates the Stack ADT from Unit 4.
     */
    std::vector<std::string> processTokensWithStack(
        const std::vector<std::string>& tokens) const;

    /*
     * calculateJaccard(setA, setB)
     * Computes Jaccard Similarity = |A ∩ B| / |A ∪ B|
     * where A and B are multisets of n-grams.
     * Range: [0.0, 1.0]
     */
    double calculateJaccard(const std::vector<std::string>& setA,
                            const std::vector<std::string>& setB,
                            std::vector<std::string>&       common) const;

public:
    /*
     * Constructor
     * ngramSize: how many consecutive words form one "phrase" for comparison.
     *            Default of 3 (trigrams) balances precision and recall.
     */
    explicit PlagiarismDetector(int ngramSize = 3);

    // Destructor – frees the DocumentStore
    ~PlagiarismDetector();

    // ----- Document Management -----

    // Load a reference document from a plain-text file
    bool loadReferenceFile(const std::string& filepath);

    // Add a reference document from an in-memory string
    bool addReferenceText(const std::string& id,
                          const std::string& name,
                          const std::string& text);

    // Show all documents currently in the store
    void listDocuments() const;

    // Remove a document by its id
    bool removeDocument(const std::string& id);

    // ----- Detection -----

    /*
     * analyse(suspectText)
     * Compares 'suspectText' against every document in the store.
     * Returns a vector of SimilarityResult sorted by descending similarity.
     * If the store is empty, returns an empty vector.
     */
    std::vector<SimilarityResult> analyse(const std::string& suspectText) const;

    // ----- Reporting -----

    /*
     * printReport(results, suspectName)
     * Prints a formatted table of all results to stdout.
     * Highlights the top match and shows matched phrases as evidence.
     */
    static void printReport(const std::vector<SimilarityResult>& results,
                            const std::string& suspectName);

    /*
     * getVerdict(similarity)
     * Returns a human-readable string classifying the similarity score.
     */
    static std::string getVerdict(double similarity);

    // Getter
    int getNgramSize() const { return ngramSize; }
};

#endif // PLAGIARISM_DETECTOR_H
