# Define the compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -g

# Define the executable output
TARGET = out

# List of source files
SOURCES = main.cpp MFERData.cpp MFERDataCollection.cpp DataStack.cpp

# Object files are the .cpp files, but with .o extensions
OBJECTS = $(SOURCES:.cpp=.o)

# The first target is the default, this is what is built when you just type 'make'
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# A rule to clean up all generated files
clean:
	rm -f $(TARGET) $(OBJECTS)

# Dependency rules for objects; how to build from .cpp
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Test for single file
# Default filename
FILE ?= ./test-file.MWF

# Test target
test: $(TARGET)
	./$(TARGET) -i $(FILE)

# Declare 'clean' and 'test' as phony targets to ensure they're always executed,
# even if files with these names exist.
.PHONY: clean test
