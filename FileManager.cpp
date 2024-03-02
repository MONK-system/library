#include "FileManager.h"
#include "ByteVector.h"
#include <iostream> // For error logging

using namespace std;

FileManager::FileManager(const string &outputFileName) : fileName(outputFileName)
{
    outputFile.open(fileName, ios::out | ios::trunc);
    if (!outputFile.is_open())
    {
        cerr << "Error opening output file: " << fileName << endl;
    }
}

void FileManager::writeLine(const string &line)
{
    if (!outputFile.is_open())
    {
        cerr << "Output file is not open." << endl;
        return;
    }

    outputFile << line << endl;
}

void FileManager::writeLines(const vector<string> &lines)
{
    for (const auto &line : lines)
    {
        writeLine(line);
    }
}

void FileManager::closeFile()
{
    if (outputFile.is_open())
    {
        outputFile.close();
    }
}

// Close the file when done, which could be handled
// in the destructor if the file should remain open for the object's lifetime.
FileManager::~FileManager()
{
    closeFile();
}

// Implementation of readBinaryFile static method
ByteVector FileManager::readBinaryFile(const string &fileName)
{
    ifstream file(fileName, ios::binary);

    if (!file.is_open())
    {
        throw runtime_error("Error opening file.");
    }

    // Read file length
    file.seekg(0, ios::end);
    unsigned long long length = file.tellg();
    file.seekg(0, ios::beg);

    // Assign bytes to vector
    ByteVector dataVector(length);
    if (!file.read(reinterpret_cast<char *>(dataVector.data()), dataVector.size()))
    {
        throw runtime_error("Error reading file.");
    }
    file.close();

    return dataVector;
}
