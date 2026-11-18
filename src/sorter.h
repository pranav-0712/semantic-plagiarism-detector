/*
 * ============================================================
 *  sorter.h  –  Similarity Results & Merge Sort
 *
 *  After computing plagiarism similarity scores against every
 *  document in the store, results are ranked using Merge Sort
 *  (highest similarity first) so the most likely plagiarism
 *  candidate appears at the top.
 * ============================================================
 */

#ifndef SORTER_H
#define SORTER_H

#include <string>
#include <vector>

/*
 * SimilarityResult
 * ----------------
 * Plain data structure (ADT) that bundles all information
 * about how closely one document matches the suspect text.
 */
struct SimilarityResult {
    std::string              docId;           // ID of the reference document
    std::string              docName;         // Human-readable name
    double                   similarity;      // Jaccard similarity [0.0 – 1.0]
    int                      matchedCount;    // Number of matching n-grams
    int                      totalSuspect;    // Total n-grams in suspect text
    std::vector<std::string> matchedPhrases;  // List of common n-grams (evidence)

    // Default constructor
    SimilarityResult()
        : similarity(0.0), matchedCount(0), totalSuspect(0) {}
};

/*
 * Sorter
 * ------
 * Provides static Merge Sort and Linear Search utilities
 * that operate on arrays of SimilarityResult.
 */
class Sorter {
public:
    /*
     * mergeSort(arr, left, right)
     * Sorts arr[left..right] in DESCENDING order of similarity score.
     *
     * Algorithm (Divide & Conquer):
     *   1. Find midpoint mid = (left + right) / 2
     *   2. Recursively sort the left  half: arr[left..mid]
     *   3. Recursively sort the right half: arr[mid+1..right]
     *   4. Merge the two sorted halves back into arr
     *
     * Time Complexity : O(n log n) in all cases
     * Space Complexity: O(n) auxiliary (for the merge step)
     */
    static void mergeSort(std::vector<SimilarityResult>& arr,
                          int left, int right);

    /*
     * linearSearch(arr, docId)
     * Performs a sequential search through 'arr' for the given docId.
     * Returns the index if found, -1 otherwise.
     * Time Complexity: O(n)
     */
    static int linearSearch(const std::vector<SimilarityResult>& arr,
                            const std::string& docId);

private:
    /*
     * merge(arr, left, mid, right)
     * Merges arr[left..mid] and arr[mid+1..right] into sorted order.
     * Uses a temporary vector as auxiliary storage.
     * Time Complexity: O(n)
     */
    static void merge(std::vector<SimilarityResult>& arr,
                      int left, int mid, int right);
};

#endif // SORTER_H
