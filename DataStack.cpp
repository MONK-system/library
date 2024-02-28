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

template <typename T>
T DataStack::pop_bytes(unsigned long long num)
{
    return hexVectorToInt<T>(pop_front(num));
}

template unsigned short DataStack::pop_bytes<unsigned short>(unsigned long long);
template unsigned DataStack::pop_bytes<unsigned>(unsigned long long);

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
T DataStack::pop_value(ByteOrder byteOrder)
{
    try
    {
        std::vector<unsigned char> hexVector = pop_front(sizeof(T));
        return hexVectorToInt<T>(hexVector, byteOrder);
    }
    catch (const std::runtime_error &e)
    {
        throw e;
    }
}

template <typename T>
std::vector<T> DataStack::pop_values(int num, ByteOrder byteOrder)
{
    std::vector<T> values;
    for (int i = 0; i < (int)num; ++i)
    {
        try
        {
            values.push_back(pop_value<T>(byteOrder));
        }
        catch (const std::runtime_error &e)
        {
            throw e;
        }
    }
    return values;
}

template std::vector<long long> DataStack::pop_values<long long>(int, ByteOrder);

template <typename T>
double DataStack::pop_double(ByteOrder byteOrder)
{
    try
    {
        return static_cast<double>(pop_value<T>(byteOrder));
    }
    catch (const std::runtime_error &e)
    {
        throw e;
    }
}

template <typename T>
std::vector<double> DataStack::pop_doubles(int num, ByteOrder byteOrder)
{
    std::vector<double> values;
    for (int i = 0; i < (int)num; ++i)
    {
        try
        {
            values.push_back(pop_double<T>(byteOrder));
        }
        catch (const std::runtime_error &e)
        {
            throw e;
        }
    }
    return values;
}

template std::vector<double> DataStack::pop_doubles<short>(int, ByteOrder);
template std::vector<double> DataStack::pop_doubles<unsigned short>(int, ByteOrder);
template std::vector<double> DataStack::pop_doubles<int>(int, ByteOrder);
template std::vector<double> DataStack::pop_doubles<unsigned char>(int, ByteOrder);
template std::vector<double> DataStack::pop_doubles<char>(int, ByteOrder);
template std::vector<double> DataStack::pop_doubles<unsigned int>(int, ByteOrder);
template std::vector<double> DataStack::pop_doubles<float>(int, ByteOrder);
template std::vector<double> DataStack::pop_doubles<double>(int, ByteOrder);