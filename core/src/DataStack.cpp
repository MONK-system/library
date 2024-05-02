#include "DataStack.h"
#include "ByteVector.h"

DataStack::DataStack(const ByteVector dataVector)
{
    try
    {
        data.insert(data.end(), dataVector.begin(), dataVector.end()); // Transfer data to deque
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error("Error while initializing DataStack: " + std::string(e.what()));
    }
}

ByteVector DataStack::pop_front(uint64_t num)
{
    if (data.size() < num)
    {
        throw std::runtime_error("Error while reading data, number out of range.");
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
    if (data.empty())
    {
        throw std::runtime_error("Error while reading data, stack is empty.");
    }
    return pop_front().toInt<uint8_t>();
}

uint8_t DataStack::read_byte() const
{
    if (data.empty())
    {
        throw std::runtime_error("Error while reading data, stack is empty.");
    }
    return data.front();
}

size_t DataStack::size() const
{
    return data.size();
}
