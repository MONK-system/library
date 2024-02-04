#include <iostream>
#include "DataStack.h"
#include "HexVector.h"

DataStack::DataStack(std::vector<unsigned char> dataVector)
{
    // Transfer data to deque
    data.insert(data.end(), dataVector.begin(), dataVector.end());
}

std::vector<unsigned char> DataStack::pop_front(unsigned long long num)
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

unsigned long long DataStack::pop_hex(unsigned char num)
{
    return hexVectorToInt<unsigned long long>(pop_front(num));
}

unsigned char DataStack::pop_byte()
{
    return hexVectorToInt<unsigned char>(pop_front());
}

size_t DataStack::size() const
{
    return data.size();
}