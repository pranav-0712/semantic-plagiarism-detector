#!/usr/bin/env pwsh
# push_to_github.ps1
# Script to initialise git and push the project to GitHub
# Run from the project root: d:\Domestix\semantic-plagiarism-detector

$REPO_URL = "https://github.com/pranav-0712/semantic-plagiarism-detector.git"

Write-Host "`n[1/5] Initialising git repository..." -ForegroundColor Cyan
git init

Write-Host "`n[2/5] Adding remote origin..." -ForegroundColor Cyan
git remote add origin $REPO_URL 2>$null
if ($LASTEXITCODE -ne 0) {
    git remote set-url origin $REPO_URL
}

Write-Host "`n[3/5] Staging all files..." -ForegroundColor Cyan
git add .
git status

Write-Host "`n[4/5] Creating initial commit..." -ForegroundColor Cyan
git commit -m "Initial commit: AI Plagiarism Detector - FDS Mini Project

- C++ implementation using 5 FDS data structures
- Stack ADT (Unit 4): stop-word filtering pipeline
- Singly Linked List (Unit 5): document database (DocumentStore)
- Naive String Matching (Unit 2): n-gram comparison engine
- Merge Sort (Unit 3): rank results by similarity
- Linear Search (Unit 3): document lookup
- Jaccard Similarity: scoring algorithm
- Interactive CLI with menu
- Static website with live JS demo
- Complete project documentation & README"

Write-Host "`n[5/5] Pushing to GitHub main branch..." -ForegroundColor Cyan
git branch -M main
git push -u origin main

if ($LASTEXITCODE -eq 0) {
    Write-Host "`n SUCCESS! Project pushed to:" -ForegroundColor Green
    Write-Host "  https://github.com/pranav-0712/semantic-plagiarism-detector" -ForegroundColor Yellow
} else {
    Write-Host "`n Push failed. You may need to authenticate with GitHub." -ForegroundColor Red
    Write-Host "  Try: git push -u origin main" -ForegroundColor Yellow
}
