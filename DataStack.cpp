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
    for (int i = 0; i < (int)num; ++i)
    {
        hexVector.push_back(data.front());
        data.pop_front();
    }
    return hexVector;
}

unsigned long long DataStack::pop_bytes(unsigned long long num)
{
    return hexVectorToInt<unsigned long long>(pop_front(num));
}

unsigned char DataStack::pop_byte()
{
    return hexVectorToInt<unsigned char>(pop_front());
}

unsigned char DataStack::read_byte() const
{
    return data.front();
}

size_t DataStack::size() const
{
    return data.size();
}

template <typename T>
T DataStack::pop_value(int size)
{
    try
    {
        std::vector<unsigned char> hexVector = pop_front(size);
        return hexVectorToInt<T>(hexVector);
    }
    catch (const std::runtime_error &e)
    {
        throw e;
    }
}

template <typename T>
std::vector<T> DataStack::pop_values(int num, int size)
{
    std::vector<T> values;
    for (int i = 0; i < (int)num; ++i)
    {
        try
        {
            values.push_back(pop_value<T>(size));
        }
        catch (const std::runtime_error &e)
        {
            throw e;
        }
    }
    return values;
}

template std::vector<long long> DataStack::pop_values<long long>(int, int);