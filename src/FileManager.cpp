#include "FileManager.h"
#include <iostream>

FileManager::FileManager(const std::string &outputFileName) : fileName(outputFileName)
{
    outputFile.open(fileName, std::ios::out | std::ios::trunc);
    if (!outputFile.is_open())
    {
        std::cerr << "Error opening output file: " << fileName << std::endl;
    }
}

void FileManager::writeLine(const std::string &line)
{
    if (!outputFile.is_open())
    {
        std::cerr << "Output file is not open." << std::endl;
        return;
    }

    outputFile << line << std::endl;
}

void FileManager::writeLines(const std::vector<std::string> &lines)
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
ByteVector FileManager::readBinaryFile(const std::string &fileName)
{
    std::ifstream file(fileName, std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("Error opening file.");
    }

    // Read file length
    file.seekg(0, std::ios::end);
    uint64_t length = file.tellg();
    file.seekg(0, std::ios::beg);

    // Assign bytes to vector
    ByteVector dataVector(length);
    if (!file.read(reinterpret_cast<char *>(dataVector.data()), dataVector.size()))
    {
        throw std::runtime_error("Error reading file.");
    }
    file.close();

    return dataVector;
}

void FileManager::writeBinaryFile(const std::string &fileName, const ByteVector &data)
{
    std::ofstream file(fileName, std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("Error opening file.");
    }

    if (!file.write(reinterpret_cast<const char *>(data.data()), data.size()))
    {
        throw std::runtime_error("Error writing file.");
    }
    file.close();
}