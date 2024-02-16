#include "FileManager.h"
#include <iostream> // For error logging

FileManager::FileManager(const std::string &outputFileName) : fileName(outputFileName), outputFormat("csv")
{
    outputFile.open(fileName);
    if (!outputFile.is_open())
    {
        std::cerr << "Error opening output file: " << fileName << std::endl;
    }
}

void FileManager::setOutputFormat(const std::string &format)
{
    outputFormat = format;
}

void FileManager::writeLines(const std::vector<std::string> &lines)
{
    if (!outputFile.is_open())
    {
        std::cerr << "Output file is not open." << std::endl;
        return;
    }

    for (const auto &line : lines)
    {
        outputFile << line << std::endl;
    }
}

void FileManager::closeFile()
{
    if (outputFile.is_open())
    {
        outputFile.close();
    }
}

// Remember to properly close the file when done, which could be handled
// in the destructor if the file should remain open for the object's lifetime.
FileManager::~FileManager()
{
    closeFile();
}

// Implementation of readBinaryFile static method
std::vector<unsigned char> FileManager::readBinaryFile(const std::string &fileName)
{
    std::ifstream file(fileName, std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("Error opening file.");
    }

    // Read file length
    file.seekg(0, std::ios::end);
    unsigned long long length = file.tellg();
    file.seekg(0, std::ios::beg);

    // Assign bytes to vector
    std::vector<unsigned char> dataVector(length);
    if (!file.read(reinterpret_cast<char *>(dataVector.data()), dataVector.size()))
    {
        throw std::runtime_error("Error reading file.");
    }
    file.close();

    return dataVector;
}
