#include "DataStack.h"
#include "ByteVector.h"
#include <iostream>

using namespace std;

DataStack::DataStack(const ByteVector dataVector)
{
    // Transfer data to deque
    data.insert(data.end(), dataVector.begin(), dataVector.end());
}

ByteVector DataStack::pop_front(uint64_t num)
{
    if (data.size() < num)
    {
        throw runtime_error("Error while reading data, number out of range.");
    }
    ByteVector byteVector;
    for (uint64_t i = 0; i < num; ++i)
    {
        byteVector.push_back(data.front());
        data.pop_front();
    }
    return byteVector;
}

uint8_t DataStack::pop_byte()
{
    return pop_front().toInt<uint8_t>();
}

uint8_t DataStack::read_byte() const
{
    return data.front();
}

size_t DataStack::size() const
{
    return data.size();
}