#ifndef DATASTACK_H
#define DATASTACK_H

#include "ByteVector.h"
#include <vector>
#include <deque>
#include <cstdint>

using namespace std;

class DataStack
{
public:
    DataStack(const ByteVector dataVector);

    size_t size() const;
    ByteVector pop_front(uint64_t num = 1);

    template <typename T>
    inline T pop_bytes(uint64_t num)
    {
        return pop_front(num).toInt<T>();
    }
    uint8_t pop_byte();
    uint8_t read_byte() const;

    template <typename T>
    inline T pop_value(ByteOrder byteOrder = ByteOrder::ENDIAN_BIG)
    {
        try
        {
            ByteVector byteVector = pop_front(sizeof(T));
            return byteVector.toInt<T>(byteOrder);
        }
        catch (const runtime_error &e)
        {
            throw e;
        }
    }
    template <typename T>
    inline vector<T> pop_values(uint64_t num, ByteOrder byteOrder = ByteOrder::ENDIAN_BIG)
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

    template <typename T>
    inline double pop_double(ByteOrder byteOrder = ByteOrder::ENDIAN_BIG)
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
    inline vector<double> pop_doubles(uint64_t num, ByteOrder byteOrder = ByteOrder::ENDIAN_BIG)
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

private:
    deque<uint8_t> data;
};

#endif