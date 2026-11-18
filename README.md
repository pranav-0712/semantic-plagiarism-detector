<div align="center">

# 🔍 Semantic Plagiarism Detector

**An AI-powered, CLI-based plagiarism detection engine built from scratch in C++17**

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue?style=for-the-badge&logo=cplusplus)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Complete-brightgreen?style=for-the-badge)
![FDS Project](https://img.shields.io/badge/FDS-Mini%20Project-orange?style=for-the-badge)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-lightgrey?style=for-the-badge)

*SY B.Tech IT | Fundamentals of Data Structures | Pranav*

</div>

---

## 📌 About

The **Semantic Plagiarism Detector** compares a suspect document against a corpus of reference documents and produces a quantitative similarity score using **N-Gram Analysis** and **Jaccard Similarity**. Every component — from storage to sorting — is implemented using fundamental data structures, making this a complete applied FDS showcase.

No external libraries. No shortcuts. Pure C++17 + Data Structures.

---

## ✨ Features

- 📂 **Dynamic Document Store** — Singly Linked List stores reference documents at runtime
- 🔤 **Text Preprocessing** — Lowercase normalization, punctuation stripping, tokenization
- 🗂️ **Stop-Word Filtering** — Stack ADT-based filter removes semantic noise
- 🔗 **Trigram Generation** — Sliding-window 3-word n-grams from token arrays
- 🔍 **Naive String Matching** — Classic Unit 2 algorithm for trigram comparison
- 📊 **Jaccard Similarity** — `|A ∩ B| / |A ∪ B|` for document similarity score
- 📈 **Merge Sort Ranking** — Results ranked in descending order of similarity
- 🏷️ **Verdict System** — HIGH / MODERATE / LOW / ORIGINAL classification
- 💻 **Interactive CLI Menu** — Load, analyze, and report with simple numbered options

---

## 🏫 FDS Concepts Applied

| FDS Unit | Concept | Location in Code | Role in Pipeline |
|---|---|---|---|
| **Unit 1** | ADT Design + Big-O Analysis | `plagiarism_detector.h/.cpp` | Core engine interface & complexity docs |
| **Unit 2** | Arrays, Strings, Naive String Matching | `string_matcher.h/.cpp` | Tokenizer, n-gram generator, trigram lookup |
| **Unit 3** | Merge Sort + Linear Search | `sorter.h/.cpp` | Rank documents by similarity score |
| **Unit 4** | Stack ADT (array-based, generic) | `stack.h` | Stop-word filter during preprocessing |
| **Unit 5** | Singly Linked List | `document_store.h/.cpp` | Reference document corpus storage |

---

## 🛠️ Installation & Build

### Prerequisites

| Platform | Requirement |
|---|---|
| Windows | MinGW-w64 (g++ 10+) or MSVC 2019+ |
| Linux / macOS | g++ 10+ or clang++ 12+ |

### Build on Windows (PowerShell)

```powershell
# Clone the repository
git clone https://github.com/pranav-0712/semantic-plagiarism-detector.git
cd semantic-plagiarism-detector

# Compile
g++ -std=c++17 -Wall -O2 `
    src/main.cpp `
    src/string_matcher.cpp `
    src/document_store.cpp `
    src/sorter.cpp `
    src/plagiarism_detector.cpp `
    -o plagiarism_detector.exe

# Run
.\plagiarism_detector.exe
```

### Build on Linux / macOS

```bash
# Clone the repository
git clone https://github.com/pranav-0712/semantic-plagiarism-detector.git
cd semantic-plagiarism-detector

# Compile
g++ -std=c++17 -Wall -O2 \
    src/main.cpp \
    src/string_matcher.cpp \
    src/document_store.cpp \
    src/sorter.cpp \
    src/plagiarism_detector.cpp \
    -o plagiarism_detector

# Run
./plagiarism_detector
```

---

## 🚀 How to Run

```
=================================================
     AI Semantic Plagiarism Detector v1.0
     FDS Mini Project | SY B.Tech IT | Pranav
=================================================

[MENU]
  1. Load Reference Documents
  2. Analyze Suspect Document
  3. View Document Store
  4. Clear All Documents
  5. Exit
Enter choice: 2

[*] Analyzing suspect.txt...

--- PLAGIARISM REPORT ---
Suspect : samples/suspect.txt
Tokens  : 312  |  Trigrams: 310

+----+-------------------------------+----------+-----------+----------+
| #  | Reference Document            | Common Ƭ | Jaccard   | Verdict  |
+----+-------------------------------+----------+-----------+----------+
|  1 | Neural Network Survey (ID:2)  |   187    |  0.6823   | HIGH 🔴  |
|  2 | Deep Learning Fundamentals(1) |    94    |  0.3105   | MOD. 🟡  |
+----+-------------------------------+----------+-----------+----------+

Top Matched Phrases (ID:2):
  → "neural networks are"
  → "gradient descent algorithm"
  → "back propagation through"

OVERALL VERDICT: 🔴 HIGH PLAGIARISM (68.23% — "Neural Network Survey")
```

---

## 📁 Project Structure

```
semantic-plagiarism-detector/
│
├── src/
│   ├── stack.h                   # 📦 Generic Stack ADT — template, array-based (Unit 4)
│   ├── string_matcher.h          # 🔤 String Matcher interface (Unit 2)
│   ├── string_matcher.cpp        # tokenize(), generateNgrams(), naiveMatch()
│   ├── document_store.h          # 🔗 Linked List node + DocumentStore class (Unit 5)
│   ├── document_store.cpp        # addDocument(), getAll(), findById()
│   ├── sorter.h                  # 📈 MergeSort declarations (Unit 3)
│   ├── sorter.cpp                # mergeSort(), merge()
│   ├── plagiarism_detector.h     # 🧠 PlagiarismDetector interface (Unit 1 ADT)
│   ├── plagiarism_detector.cpp   # analyze(), computeJaccard()
│   └── main.cpp                  # 💻 CLI menu, entry point
│
├── docs/
│   └── report.md                 # 📄 Full FDS project report
│
├── samples/
│   ├── ref1.txt                  # Reference document 1
│   ├── ref2.txt                  # Reference document 2
│   └── suspect.txt               # Sample suspect document
│
├── README.md
└── Makefile
```

---

## 🧠 Algorithm Overview

```
Raw Text Files
     │
     ▼
[DocumentStore — Singly Linked List]
     │  addDocument() → O(1)
     ▼
[StringMatcher — Tokenizer]
     │  lowercase + strip punctuation → token array
     ▼
[Stack ADT — Stop-Word Filter]
     │  push/pop tokens → filtered array  → O(n)
     ▼
[N-Gram Generator — Sliding Window]
     │  trigrams set  → O(n)
     ▼
[Naive String Matching — Intersection]
     │  common trigrams → O(n·m)
     ▼
[Jaccard Similarity]
     │  score = |A∩B| / |A∪B|
     ▼
[Merge Sort — Ranking]
     │  descending by score → O(n log n)
     ▼
[Report + Verdict]
     HIGH ≥60% | MODERATE ≥30% | LOW >0% | ORIGINAL =0%
```

---

## 📊 Sample Results

| Test Case | Suspect Type | Jaccard Score | Verdict |
|---|---|---|---|
| TC-1 | Near-verbatim copy | 68.23% | 🔴 HIGH |
| TC-2 | Paraphrased content | 31.05% | 🟡 MODERATE |
| TC-3 | Unrelated document | 0.00% | ✅ ORIGINAL |

---

## 📄 License

This project is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for details.

---

## 👨‍💻 Author

**Pranav**  
SY B.Tech Information Technology  
FDS Mini Project | November 2026  
🔗 [github.com/pranav-0712/semantic-plagiarism-detector](https://github.com/pranav-0712/semantic-plagiarism-detector)

---

<div align="center">

*Built with ❤️ and pure Data Structures — no shortcuts, no external libraries*

</div>
