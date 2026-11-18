/*
 * ============================================================
 *  string_matcher.h  –  String Processing & Naive Matching
 *
 *  Provides utilities for:
 *   - Text normalization (lowercase, punctuation removal)
 *   - Tokenization (splitting text into words)
 *   - N-gram generation (sliding window of n consecutive words)
 *   - Naive String Matching (brute-force pattern search)
 * ============================================================
 */

#ifndef STRING_MATCHER_H
#define STRING_MATCHER_H

#include <string>
#include <vector>

class StringMatcher {
public:
    /*
     * toLowerCase(s)
     * Converts every character in string s to its lowercase equivalent.
     * Time Complexity: O(n) where n = length of string
     */
    static std::string toLowerCase(const std::string& s);

    /*
     * removePunctuation(s)
     * Strips all punctuation and non-alphanumeric characters from s,
     * replacing them with a space so word boundaries are preserved.
     * Time Complexity: O(n)
     */
    static std::string removePunctuation(const std::string& s);

    /*
     * normalize(s)
     * Convenience wrapper: applies removePunctuation then toLowerCase.
     * Time Complexity: O(n)
     */
    static std::string normalize(const std::string& s);

    /*
     * tokenize(text)
     * Splits 'text' on whitespace and returns a vector of word tokens.
     * Empty tokens (caused by multiple spaces) are ignored.
     * Time Complexity: O(n)
     */
    static std::vector<std::string> tokenize(const std::string& text);

    /*
     * generateNGrams(tokens, n)
     * Builds a sliding-window list of n-grams from a token array.
     * Each n-gram is n consecutive words joined by a single space.
     *   e.g. tokens={"the","cat","sat"}, n=2  →  {"the cat", "cat sat"}
     * Time Complexity: O(k * n) where k = number of tokens
     */
    static std::vector<std::string> generateNGrams(
        const std::vector<std::string>& tokens, int n);

    /*
     * naiveSearch(text, pattern)
     * Implements the classic Naive (Brute-Force) String Matching algorithm.
     * Returns the index of the FIRST occurrence of 'pattern' inside 'text',
     * or -1 if no match is found.
     *
     * Algorithm:
     *   For every position i in text[0 .. |text|-|pattern|]:
     *     Compare text[i..i+|pattern|-1] with pattern character by character.
     *     If all characters match → return i.
     *
     * Time Complexity: O(n * m) where n=|text|, m=|pattern|
     */
    static int naiveSearch(const std::string& text, const std::string& pattern);

    /*
     * naiveSearchAll(text, pattern)
     * Like naiveSearch but returns ALL starting positions of 'pattern' in 'text'.
     * Time Complexity: O(n * m)
     */
    static std::vector<int> naiveSearchAll(
        const std::string& text, const std::string& pattern);

    /*
     * countNGramMatches(sourceNGrams, suspectNGrams)
     * Counts how many n-grams from 'suspectNGrams' appear in 'sourceNGrams'.
     * Uses naiveSearch internally to test membership.
     * Time Complexity: O(S * P * m) where S=|source|, P=|suspect|, m=avg ngram length
     */
    static int countNGramMatches(
        const std::vector<std::string>& sourceNGrams,
        const std::vector<std::string>& suspectNGrams,
        std::vector<std::string>&       matchedNGrams);
};

#endif // STRING_MATCHER_H
