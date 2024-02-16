#include "FileManager.h"

// Implementation of readBinaryFile static method
std::vector<unsigned char> FileManager::readBinaryFile(const std::string& fileName) {
    std::ifstream file(fileName, std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Error opening file.");
    }

    // Read file length
    file.seekg(0, std::ios::end);
    unsigned long long length = file.tellg();
    file.seekg(0, std::ios::beg);

    // Assign bytes to vector
    std::vector<unsigned char> dataVector(length);
    if (!file.read(reinterpret_cast<char *>(dataVector.data()), dataVector.size())) {
        throw std::runtime_error("Error reading file.");
    }
    file.close();

    return dataVector;
}
