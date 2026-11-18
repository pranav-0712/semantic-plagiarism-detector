/*
 * ============================================================
 *  string_matcher.cpp  –  Implementation of StringMatcher
 * ============================================================
 */

#include "string_matcher.h"
#include <algorithm>
#include <cctype>
#include <sstream>

// ---------- toLowerCase ----------
std::string StringMatcher::toLowerCase(const std::string& s) {
    std::string result = s;
    // Traverse each character and convert to lowercase using tolower()
    for (char& c : result)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return result;
}

// ---------- removePunctuation ----------
std::string StringMatcher::removePunctuation(const std::string& s) {
    std::string result;
    result.reserve(s.size());
    for (char c : s) {
        // Keep alphanumeric characters and spaces; replace everything else with space
        if (std::isalnum(static_cast<unsigned char>(c)) || c == ' ')
            result += c;
        else
            result += ' ';
    }
    return result;
}

// ---------- normalize ----------
std::string StringMatcher::normalize(const std::string& s) {
    // Step 1: Remove punctuation → Step 2: Convert to lowercase
    return toLowerCase(removePunctuation(s));
}

// ---------- tokenize ----------
std::vector<std::string> StringMatcher::tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    std::istringstream stream(text);
    std::string word;
    // std::istringstream splits on whitespace automatically
    while (stream >> word) {
        if (!word.empty())
            tokens.push_back(word);
    }
    return tokens;
}

// ---------- generateNGrams ----------
std::vector<std::string> StringMatcher::generateNGrams(
        const std::vector<std::string>& tokens, int n) {

    std::vector<std::string> ngrams;
    int totalTokens = static_cast<int>(tokens.size());

    // Need at least n tokens to form even one n-gram
    if (totalTokens < n)
        return ngrams;

    // Sliding window: position i to i+n-1
    for (int i = 0; i <= totalTokens - n; i++) {
        std::string ngram = tokens[i];
        for (int j = 1; j < n; j++) {
            ngram += " " + tokens[i + j];   // Join words with a space
        }
        ngrams.push_back(ngram);
    }
    return ngrams;
}

// ---------- naiveSearch ----------
/*
 * Naive (Brute-Force) String Matching
 * ------------------------------------
 * For each starting position i in the text:
 *   Try to match every character of the pattern.
 *   If all match → found at index i.
 *   If any character mismatches → move i forward by 1.
 */
int StringMatcher::naiveSearch(const std::string& text,
                               const std::string& pattern) {
    int n = static_cast<int>(text.size());
    int m = static_cast<int>(pattern.size());

    if (m == 0) return 0;   // Empty pattern trivially matches at position 0
    if (m > n)  return -1;  // Pattern longer than text → no match possible

    for (int i = 0; i <= n - m; i++) {
        int j = 0;
        // Character-by-character comparison
        while (j < m && text[i + j] == pattern[j])
            j++;

        if (j == m)
            return i;   // All m characters matched
    }
    return -1;  // No match found
}

// ---------- naiveSearchAll ----------
std::vector<int> StringMatcher::naiveSearchAll(const std::string& text,
                                               const std::string& pattern) {
    std::vector<int> positions;
    int n = static_cast<int>(text.size());
    int m = static_cast<int>(pattern.size());

    if (m == 0 || m > n) return positions;

    for (int i = 0; i <= n - m; i++) {
        int j = 0;
        while (j < m && text[i + j] == pattern[j])
            j++;

        if (j == m)
            positions.push_back(i);     // Record every occurrence
    }
    return positions;
}

// ---------- countNGramMatches ----------
int StringMatcher::countNGramMatches(
        const std::vector<std::string>& sourceNGrams,
        const std::vector<std::string>& suspectNGrams,
        std::vector<std::string>&       matchedNGrams) {

    int matchCount = 0;
    // Reconstruct source as a single searchable string for efficiency
    // (each n-gram is separated by a unique sentinel so partial matches don't cross n-gram boundaries)
    std::string sourceBlob;
    for (const std::string& ng : sourceNGrams)
        sourceBlob += "|" + ng + "|";   // "|" acts as a delimiter/sentinel

    // For each suspect n-gram, check if it exists anywhere in the source blob
    for (const std::string& suspectNG : suspectNGrams) {
        std::string searchTarget = "|" + suspectNG + "|";
        if (naiveSearch(sourceBlob, searchTarget) != -1) {
            matchedNGrams.push_back(suspectNG);
            matchCount++;
        }
    }
    return matchCount;
}
