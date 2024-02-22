#ifndef DATASTACK_H
#define DATASTACK_H

#include <fstream>
#include <vector>
#include <deque>

class DataStack
{
public:
    DataStack(std::vector<unsigned char> dataVector);

    size_t size() const;
    std::vector<unsigned char> pop_front(unsigned long long num = 1);
    unsigned long long pop_bytes(unsigned long long num);
    unsigned char pop_byte();
    unsigned char read_byte() const;
    template <typename T>
    T pop_value(int size = sizeof(T));
    template <typename T>
    std::vector<T> pop_values(int num, int size = sizeof(T));

private:
    std::deque<unsigned char> data;
};

#endif