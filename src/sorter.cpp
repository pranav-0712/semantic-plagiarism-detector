/*
 * ============================================================
 *  sorter.cpp  –  Implementation of Merge Sort & Linear Search
 * ============================================================
 */

#include "sorter.h"
#include <vector>

// ============================================================
//  mergeSort  –  Entry point (recursive)
// ============================================================
void Sorter::mergeSort(std::vector<SimilarityResult>& arr,
                       int left, int right) {
    // Base case: a subarray of size 0 or 1 is already sorted
    if (left >= right) return;

    // Step 1: Find the midpoint
    int mid = left + (right - left) / 2;   // Avoids integer overflow vs (l+r)/2

    // Step 2: Recursively sort each half
    mergeSort(arr, left,    mid);
    mergeSort(arr, mid + 1, right);

    // Step 3: Merge the two sorted halves
    merge(arr, left, mid, right);
}

// ============================================================
//  merge  –  Combine two sorted halves (descending by similarity)
// ============================================================
void Sorter::merge(std::vector<SimilarityResult>& arr,
                   int left, int mid, int right) {

    // Copy both halves into temporary vectors
    std::vector<SimilarityResult> leftArr (arr.begin() + left,    arr.begin() + mid + 1);
    std::vector<SimilarityResult> rightArr(arr.begin() + mid + 1, arr.begin() + right + 1);

    int i = 0;                  // Index into leftArr
    int j = 0;                  // Index into rightArr
    int k = left;               // Index into original arr

    // Merge: always pick the element with the HIGHER similarity first (descending)
    while (i < (int)leftArr.size() && j < (int)rightArr.size()) {
        if (leftArr[i].similarity >= rightArr[j].similarity) {
            arr[k++] = leftArr[i++];
        } else {
            arr[k++] = rightArr[j++];
        }
    }

    // Copy any remaining elements from leftArr
    while (i < (int)leftArr.size())
        arr[k++] = leftArr[i++];

    // Copy any remaining elements from rightArr
    while (j < (int)rightArr.size())
        arr[k++] = rightArr[j++];
}

// ============================================================
//  linearSearch  –  Sequential search by docId
// ============================================================
int Sorter::linearSearch(const std::vector<SimilarityResult>& arr,
                         const std::string& docId) {
    // Check every element from start to end
    for (int i = 0; i < (int)arr.size(); i++) {
        if (arr[i].docId == docId)
            return i;   // Found at index i
    }
    return -1;          // Not found
}
