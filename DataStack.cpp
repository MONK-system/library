#include <iostream>
#include "DataStack.h"
#include "HexVector.h"
#include <cstdint>

using namespace std;

DataStack::DataStack(vector<unsigned char> dataVector)
{
    // Transfer data to deque
    data.insert(data.end(), dataVector.begin(), dataVector.end());
}

vector<unsigned char> DataStack::pop_front(unsigned long long num)
{
    if (data.size() < num)
    {
        throw runtime_error("Error while reading data, number out of range.");
    }
    vector<unsigned char> hexVector;
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
        vector<unsigned char> hexVector = pop_front(sizeof(T));
        return hexVectorToInt<T>(hexVector, byteOrder);
    }
    catch (const runtime_error &e)
    {
        throw e;
    }
}

template <typename T>
vector<T> DataStack::pop_values(uint64_t num, ByteOrder byteOrder)
{
    vector<T> values;
    for (int i = 0; i < (int)num; ++i)
    {
        try
        {
            values.push_back(pop_value<T>(byteOrder));
        }
        catch (const runtime_error &e)
        {
            throw e;
        }
    }
    return values;
}

template vector<long long> DataStack::pop_values<long long>(uint64_t, ByteOrder);

template <typename T>
double DataStack::pop_double(ByteOrder byteOrder)
{
    try
    {
        return static_cast<double>(pop_value<T>(byteOrder));
    }
    catch (const runtime_error &e)
    {
        throw e;
    }
}

template <typename T>
vector<double> DataStack::pop_doubles(uint64_t num, ByteOrder byteOrder)
{
    vector<double> values;
    for (uint64_t i = 0; i < num; ++i)
    {
        try
        {
            values.push_back(pop_double<T>(byteOrder));
        }
        catch (const runtime_error &e)
        {
            throw e;
        }
    }
    return values;
}

template vector<double> DataStack::pop_doubles<int16_t>(uint64_t, ByteOrder);
template vector<double> DataStack::pop_doubles<uint16_t>(uint64_t, ByteOrder);
template vector<double> DataStack::pop_doubles<int32_t>(uint64_t, ByteOrder);
template vector<double> DataStack::pop_doubles<uint8_t>(uint64_t, ByteOrder);
template vector<double> DataStack::pop_doubles<int8_t>(uint64_t, ByteOrder);
template vector<double> DataStack::pop_doubles<uint32_t>(uint64_t, ByteOrder);
template vector<double> DataStack::pop_doubles<float>(uint64_t, ByteOrder);
template vector<double> DataStack::pop_doubles<double>(uint64_t, ByteOrder);