#ifndef DATASTACK_H
#define DATASTACK_H

#include <fstream>
#include <vector>
#include <deque>
#include <cstdint>
#include "ByteVector.h"

using namespace std;

class DataStack
{
public:
    DataStack(const ByteVector dataVector);

    size_t size() const;
    ByteVector pop_front(unsigned long long num = 1);
    template <typename T>
    T pop_bytes(unsigned long long num);
    unsigned char pop_byte();
    unsigned char read_byte() const;
    template <typename T>
    T pop_value(ByteOrder byteOrder = ByteOrder::ENDIAN_BIG);
    template <typename T>
    vector<T> pop_values(uint64_t num, ByteOrder byteOrder = ByteOrder::ENDIAN_BIG);
    template <typename T>
    double pop_double(ByteOrder byteOrder = ByteOrder::ENDIAN_BIG);
    template <typename T>
    vector<double> pop_doubles(uint64_t num, ByteOrder byteOrder = ByteOrder::ENDIAN_BIG);

private:
    deque<unsigned char> data;
};

#endif