# Compiler and flags
CXX     = g++
CXXFLAGS = -std=c++17 -O2 -Wall

# Output binary name
TARGET  = plagiarism_detector

# Source files (all .cpp in src/)
SRCS    = src/main.cpp \
          src/string_matcher.cpp \
          src/document_store.cpp \
          src/sorter.cpp \
          src/plagiarism_detector.cpp

# Default target: compile everything
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)
	@echo "Build successful: $(TARGET)"

# Run the demo automatically
demo: $(TARGET)
	@echo "--- Running Demo ---"
	@echo "7\n0" | ./$(TARGET)

# Remove compiled binary
clean:
	rm -f $(TARGET) $(TARGET).exe

.PHONY: all demo clean
