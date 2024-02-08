# Define the compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -g

# Specify remove command for Windows and define the executable output based on the OS
ifeq ($(OS),Windows_NT)
    RM = cmd /C del /Q /F
    TARGET = out.exe
else
    RM = rm -f
    TARGET = out
endif

# List of source files
SOURCES = main.cpp MFERData.cpp MFERDataCollection.cpp DataStack.cpp

# Object files are the .cpp files, but with .o extensions
OBJECTS = $(SOURCES:.cpp=.o)

# The first target is the default, this is what is built when you just type 'make'
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# A rule to clean up all generated files
clean:
	-$(RM) $(TARGET) $(OBJECTS)

# Dependency rules for objects; how to build from .cpp
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Test for single file
# Default filename
FILE ?= ./test-file.MWF

# Test target
test: $(TARGET)
	./$(TARGET) -i $(FILE)

# Define a rule that compiles and then tests the application
all: $(TARGET) test

# Declare rules as phony targets to ensure they're always executed,
# even if files with these names exist.
.PHONY: clean test all
