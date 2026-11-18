# Semantic Plagiarism Detector

**Find copied content in seconds.** Compare a document against any number of sources and get a similarity score for each source, a clear verdict, and every matching passage highlighted.

**[Live demo](https://pranav-0712.github.io/semantic-plagiarism-detector/)** · [How it works](#how-it-works) · [Run the desktop engine](#desktop-engine-c)

![Semantic Plagiarism Detector](docs/assets/preview.png)

---

## Overview

Semantic Plagiarism Detector consists of two parts that share the same detection algorithm:

| Component | Description |
|---|---|
| **Web app** ([`docs/index.html`](docs/index.html)) | A single-page application that runs entirely in the browser. No installation, no account, and no text is ever uploaded. |
| **Desktop engine** ([`src/`](src)) | A dependency-free C++17 command-line application for managing a reference library and checking documents offline. |

## Features

- **Many sources, one check.** Compare a document against multiple sources at once. Results are ranked from most to least similar.
- **See exactly what matched.** Matching passages are highlighted in the document, with a full list of shared phrases per source.
- **Private by design.** All processing happens locally. Documents never leave the device.
- **Meaningful scores.** Common words such as "the" and "of" are ignored, so scores reflect shared content rather than shared grammar.
- **Adjustable precision.** Choose a phrase length of 2 to 5 words.
- **Downloadable reports.** Export a plain-text report with scores, verdicts and matched phrases.
- **File support.** Paste text, or upload and drag in `.txt` files.

![Results view](docs/assets/results.png)

## How it works

1. **Clean.** Text is lowercased, punctuation is removed, and common words and words of two letters or fewer are filtered out.
2. **Split.** The remaining words are broken into overlapping phrases (word n-grams, three words long by default).
3. **Match.** Every phrase in the document is looked up in each source.
4. **Score.** Each source receives a Jaccard similarity score, and sources are ranked from highest to lowest.

```
similarity = |A ∩ B| / |A ∪ B| = shared / (|A| + |B| − shared)
```

where `A` and `B` are the phrase sets of the source and the document.

| Score | Verdict | Meaning |
|---|---|---|
| 25% and above | Likely copied | Large portions of the document match the source. |
| 10% to 25% | Needs review | Several passages match. Check the highlighted text. |
| Below 10% | Minor overlap | A few shared phrases, typical for documents on the same subject. |
| 0% | No overlap | No shared phrases. |

Detection works on word sequences. It finds copied and lightly edited text, including reordered sentences and small word changes. Text rewritten entirely in different words scores lower.

## Data structures

The engine is built on core data structures, each implemented from scratch.

| Data structure | Where | Purpose |
|---|---|---|
| Singly linked list | `document_store.h/.cpp` | Stores the reference library. Documents can be added and removed at runtime. |
| Stack (generic, array-based) | `stack.h` | Filters common words from the token stream. |
| Arrays and strings | `string_matcher.h/.cpp` | Normalisation, tokenisation and n-gram generation. |
| Naive string matching | `string_matcher.cpp` | Finds each document phrase in the reference text. |
| Merge sort | `sorter.h/.cpp` | Ranks results by similarity, highest first. |
| Linear search | `sorter.cpp`, `document_store.cpp` | Looks up documents and results by ID. |

## Getting started

### Web app

Open [the live demo](https://pranav-0712.github.io/semantic-plagiarism-detector/), or open `docs/index.html` directly in any modern browser. No server or build step is needed. Click **Load example** to see a sample result.

### Desktop engine (C++)

Requires a C++17 compiler (g++ 10+, clang++ 12+ or MSVC 2019+).

```bash
git clone https://github.com/pranav-0712/semantic-plagiarism-detector.git
cd semantic-plagiarism-detector
make
./plagiarism_detector
```

Without `make`:

```bash
g++ -std=c++17 -O2 -Wall -o plagiarism_detector src/*.cpp
```

On Windows, run `plagiarism_detector.exe`.

The interactive menu lets you:

```
1. Load reference document from file
2. Add reference text (manual input)
3. List all reference documents
4. Remove a reference document
5. Check a text file for plagiarism
6. Check typed text for plagiarism
7. Run demo with sample documents
0. Exit
```

Sample documents for testing are in [`samples/`](samples).

## Project structure

```
semantic-plagiarism-detector/
├── docs/
│   ├── index.html              Web app (served by GitHub Pages)
│   ├── assets/                 Screenshots
│   └── report.md               Project report
├── samples/                    Sample reference and suspect documents
├── src/
│   ├── main.cpp                Command-line interface
│   ├── plagiarism_detector.*   Detection engine
│   ├── document_store.*        Linked-list document library
│   ├── string_matcher.*        Text processing and matching
│   ├── sorter.*                Merge sort and linear search
│   └── stack.h                 Generic stack
└── Makefile
```

## Roadmap

- Hash-based or KMP matching for faster lookups on large libraries
- Synonym handling to detect paraphrased text
- PDF and DOCX import

## Author

**Pranav** · [github.com/pranav-0712](https://github.com/pranav-0712)
