#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include "MFERDataCollection.h"

std::vector<unsigned char> readHexFile(std::string fileName);

int main(int argc, char *argv[])
{
    if (argc != 3 || std::string(argv[1]) != "-i")
    {
        std::cerr << "Usage: " << argv[0] << " -i <Filename>" << std::endl;
        return 1;
    }

    std::string filename = argv[2];

    std::vector<unsigned char> dataVector;

    try
    {
        dataVector = readHexFile(filename);
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Failed to read file: " << e.what() << std::endl;
        return 1;
    }

    MFERDataCollection dataCollection(dataVector);

    std::cout << std::endl << dataCollection.toString() << std::endl;

    return 0;
}

std::vector<unsigned char> readHexFile(std::string fileName)
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