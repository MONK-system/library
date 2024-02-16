#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include "MFERDataCollection.h"
#include "FileManager.h"

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
        dataVector = FileManager::readBinaryFile(filename);
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