#include <iostream>
#include "DataBlock.h"
#include "HexVector.h"

DataBlock::DataBlock(std::vector<unsigned char> dataVector)
{
    // Transfer data to deque
    data.insert(data.end(), dataVector.begin(), dataVector.end());
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