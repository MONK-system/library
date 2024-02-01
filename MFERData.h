#ifndef MFERDATA_H
#define MFERDATA_H

#include <vector>
#include <fstream>
#include <stdexcept>

class MFERData
{
public:
    MFERData(std::ifstream *data);

    static std::string headerString();
    std::string toString(int maxByteLength) const;

    const std::vector<unsigned char> &getTag() const
    {
        return tag;
    };
    const std::vector<unsigned char> &getLength() const;
    const std::vector<unsigned char> &getContents() const;

private:
    void parseData(std::ifstream *data);

    std::vector<unsigned char> tag;
    std::vector<unsigned char> length;
    std::vector<unsigned char> contents;
};

int hexVectorToInt(const std::vector<unsigned char> &hexVector);

#endif // MFERDATA_H
