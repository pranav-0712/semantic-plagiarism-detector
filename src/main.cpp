/*
 * ============================================================
 *  main.cpp  –  Semantic Plagiarism Detector (CLI entry point)
 *
 *  Interactive command-line interface for loading reference
 *  documents and checking text against them.
 *
 *  Build:
 *    make
 *    (or compile every file in src/ with g++ -std=c++17)
 *
 *  How to run:
 *    ./plagiarism_detector          (Linux/macOS)
 *    plagiarism_detector.exe        (Windows)
 * ============================================================
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <limits>

#include "plagiarism_detector.h"

// ---- Utility: Display a styled banner ----
static void printBanner() {
    std::cout << "\n";
    std::cout << "  ╔══════════════════════════════════════════════════════╗\n";
    std::cout << "  ║         SEMANTIC PLAGIARISM DETECTOR  v1.0          ║\n";
    std::cout << "  ║   N-gram matching  ·  Jaccard similarity scoring    ║\n";
    std::cout << "  ╚══════════════════════════════════════════════════════╝\n\n";
}

// ---- Utility: Read all text from a file ----
static bool readFile(const std::string& path, std::string& content) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "  [Error] Cannot open file: " << path << "\n";
        return false;
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    content = ss.str();
    return true;
}

// ---- Utility: Clear input buffer ----
static void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// ============================================================
//  DEMO  –  Loads sample files and runs a quick demonstration
// ============================================================
static void runDemo(PlagiarismDetector& detector) {
    std::cout << "\n  [Demo] Loading sample reference documents...\n";

    // ---- Sample documents loaded as strings (no file dependency for demo) ----
    std::string ref1 =
        "Machine learning is a branch of artificial intelligence that enables systems "
        "to learn from data and improve their performance over time without being "
        "explicitly programmed. It uses algorithms to parse data, learn from it, and "
        "make informed decisions. Machine learning is applied in many fields including "
        "computer vision, natural language processing, and speech recognition.";

    std::string ref2 =
        "Data structures are specialised formats for organising, processing, retrieving "
        "and storing data. They make it easier to manipulate data efficiently. Common "
        "data structures include arrays, linked lists, stacks, queues, trees and graphs. "
        "Choosing the right data structure for a problem is key to writing efficient algorithms.";

    std::string suspect1 =
        "Machine learning is a subfield of artificial intelligence that allows systems "
        "to learn from data and improve over time without being explicitly programmed. "
        "It uses algorithms to analyse data and make informed decisions based on that data.";

    std::string suspect2 =
        "Neural networks are computational models inspired by the human brain. "
        "They consist of layers of interconnected nodes that process information "
        "and can recognise complex patterns in data.";

    detector.addReferenceText("ref1", "ML_Overview.txt",        ref1);
    detector.addReferenceText("ref2", "DataStructures_Intro.txt", ref2);

    std::cout << "\n  Reference documents loaded:\n";
    detector.listDocuments();

    // ---- Run analysis on suspect1 (should show high similarity with ref1) ----
    std::cout << "  [Demo] Analysing Suspect Document 1 (copied from ML reference)...\n";
    auto results1 = detector.analyse(suspect1);
    PlagiarismDetector::printReport(results1, "Suspect_1_ML_Copy.txt");

    // ---- Run analysis on suspect2 (should show low similarity with both) ----
    std::cout << "  [Demo] Analysing Suspect Document 2 (original content)...\n";
    auto results2 = detector.analyse(suspect2);
    PlagiarismDetector::printReport(results2, "Suspect_2_Neural_Net.txt");
}

// ============================================================
//  MAIN
// ============================================================
int main() {
    printBanner();

    // Create the detector with default trigrams (n=3)
    PlagiarismDetector detector(3);

    int choice = -1;

    while (choice != 0) {
        // Update menu line 8 with current n-gram size
        std::cout << "  ┌─────────────────────────────────────────┐\n";
        std::cout << "  │               MAIN MENU                 │\n";
        std::cout << "  ├─────────────────────────────────────────┤\n";
        std::cout << "  │  1. Load reference document from file   │\n";
        std::cout << "  │  2. Add reference text (manual input)   │\n";
        std::cout << "  │  3. List all reference documents        │\n";
        std::cout << "  │  4. Remove a reference document         │\n";
        std::cout << "  │  5. Check a text file for plagiarism    │\n";
        std::cout << "  │  6. Check typed text for plagiarism     │\n";
        std::cout << "  │  7. Run demo with sample documents      │\n";
        std::cout << "  │  0. Exit                                │\n";
        std::cout << "  └─────────────────────────────────────────┘\n";
        std::cout << "  [N-gram size = " << detector.getNgramSize() << "]  Enter choice: ";

        std::cin >> choice;
        clearInput();

        switch (choice) {

            // ---- Option 1: Load reference file ----
            case 1: {
                std::cout << "  Enter file path: ";
                std::string path;
                std::getline(std::cin, path);
                if (detector.loadReferenceFile(path))
                    std::cout << "  [OK] Document loaded successfully.\n\n";
                break;
            }

            // ---- Option 2: Add reference text manually ----
            case 2: {
                std::cout << "  Enter document ID  : ";
                std::string id;   std::getline(std::cin, id);
                std::cout << "  Enter document name: ";
                std::string name; std::getline(std::cin, name);
                std::cout << "  Enter text (press Enter twice when done):\n  > ";

                std::string line, text;
                while (std::getline(std::cin, line)) {
                    if (line.empty()) break;
                    text += line + " ";
                }
                if (detector.addReferenceText(id, name, text))
                    std::cout << "  [OK] Document '" << name << "' added.\n\n";
                break;
            }

            // ---- Option 3: List documents ----
            case 3: {
                detector.listDocuments();
                break;
            }

            // ---- Option 4: Remove a document ----
            case 4: {
                std::cout << "  Enter document ID to remove: ";
                std::string id; std::getline(std::cin, id);
                if (detector.removeDocument(id))
                    std::cout << "  [OK] Document '" << id << "' removed.\n\n";
                else
                    std::cout << "  [Error] Document '" << id << "' not found.\n\n";
                break;
            }

            // ---- Option 5: Check a file for plagiarism ----
            case 5: {
                std::cout << "  Enter path to suspect file: ";
                std::string path; std::getline(std::cin, path);
                std::string content;
                if (readFile(path, content)) {
                    // Extract filename for the report
                    std::string fname = path;
                    size_t pos = fname.find_last_of("/\\");
                    if (pos != std::string::npos) fname = fname.substr(pos + 1);

                    auto results = detector.analyse(content);
                    PlagiarismDetector::printReport(results, fname);
                }
                break;
            }

            // ---- Option 6: Check typed text for plagiarism ----
            case 6: {
                std::cout << "  Enter suspect text (press Enter twice when done):\n  > ";
                std::string line, text;
                while (std::getline(std::cin, line)) {
                    if (line.empty()) break;
                    text += line + " ";
                }
                auto results = detector.analyse(text);
                PlagiarismDetector::printReport(results, "Typed Input");
                break;
            }

            // ---- Option 7: Run built-in demo ----
            case 7: {
                runDemo(detector);
                break;
            }

            case 0: {
                std::cout << "\n  Thank you for using Semantic Plagiarism Detector.\n\n";
                break;
            }

            default: {
                std::cout << "  [Error] Invalid choice. Please enter 0-7.\n\n";
                break;
            }
        }
    }

    return 0;
}
