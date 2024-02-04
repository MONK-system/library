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
    unsigned long long pop_hex(unsigned char num);
    unsigned char pop_byte();

private:
    std::deque<unsigned char> data;
};

#endif