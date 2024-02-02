#include <iostream>
#include "DataBlock.h"
#include "HexVector.h"

DataBlock::DataBlock(std::vector<unsigned char> vectorData)
{
    // Transfer data to deque
    data.insert(data.end(), vectorData.begin(), vectorData.end());
}

DataBlock::DataBlock(std::ifstream *file)
{
    // Read file length
    file->seekg(0, std::ios::end);
    unsigned long long length = file->tellg();
    file->seekg(0, std::ios::beg);

    // Assign bytes to vector
    std::vector<unsigned char> vectorData(length);
    if (!file->read(reinterpret_cast<char *>(vectorData.data()), vectorData.size()))
    {
        throw std::runtime_error("Error reading file.");
    }

    // Transfer data to deque
    data.insert(data.end(), vectorData.begin(), vectorData.end());
}

std::vector<unsigned char> DataBlock::pop_front(unsigned long long num)
{
    if (data.size() < num)
    {
        throw std::runtime_error("Error while reading data, number out of range.");
    }
    std::vector<unsigned char> hexVector;
    for (int i = 0; i < num; ++i)
    {
        hexVector.push_back(data.front());
        data.pop_front();
    }
    return hexVector;
}

unsigned long long DataBlock::pop_hex(unsigned char num)
{
    return hexVectorToInt<unsigned long long>(pop_front(num));
}

unsigned char DataBlock::pop_byte()
{
    return hexVectorToInt<unsigned char>(pop_front());
}

size_t DataBlock::size() const
{
    return data.size();
}