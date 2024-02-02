#ifndef DATABLOCK_H
#define DATABLOCK_H

#include <fstream>
#include <vector>
#include <deque>

class DataBlock
{
public:
    DataBlock(std::vector<unsigned char> vectorData);
    DataBlock(std::ifstream *file);

    size_t size() const;
    std::vector<unsigned char> pop_front(unsigned long long num = 1);
    unsigned long long pop_hex(unsigned char num);
    unsigned char pop_byte();

private:
    std::deque<unsigned char> data;
};

#endif