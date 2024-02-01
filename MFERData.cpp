#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include "MFERData.h"

MFERData::MFERData(std::ifstream *data)
{
    try
    {
        parseData(data);
    }
    catch (const std::runtime_error &e)
    {
        throw std::runtime_error("Constructor failed: " + std::string(e.what()));
    }
}

std::string MFERData::headerString()
{
    return "| Tag    | Length      | Contents \n"
           "|--------|-------------|------------>";
}

std::string MFERData::toString(int maxByteLength) const
{
    std::ostringstream tagString;
    for (auto val : tag)
    {
        tagString << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)val << " ";
    }
    tagString << std::string(6 - tag.size() * 3, ' ');

    std::ostringstream lengthString;
    for (auto val : length)
    {
        lengthString << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)val << " ";
    }
    lengthString << std::string(12 - length.size() * 3, ' ');

    std::ostringstream contentsString;
    if (contents.size() <= maxByteLength)
    {
        for (auto val : contents)
        {
            contentsString << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)val << " ";
        }
    }
    else
    {
        contentsString << "...";
    }

    std::ostringstream output;
    output << "| " << tagString.str() << "| " << lengthString.str() << "| " << contentsString.str();
    return output.str();
}

void MFERData::parseData(std::ifstream *data)
{

    // Read Tag (first byte)
    unsigned char firstTagByte;
    if (!data->read(reinterpret_cast<char *>(&firstTagByte), sizeof(firstTagByte)))
    {
        if (!data->eof())
        {
            throw std::runtime_error("Error reading tag from file.");
        }
    }

    if (firstTagByte == 0x80)
    {
        tag.push_back(firstTagByte);
        length.push_back(0x00);
        return;
    }

    if (firstTagByte == 0x00)
    {
        throw std::runtime_error("Encountered invalid tag (00), stopping read.");
    }

    tag.push_back(firstTagByte);

    // Special case for tag 3F: read next byte as part of the tag
    if (firstTagByte == 0x3F)
    {
        unsigned char secondTagByte;
        if (!data->read(reinterpret_cast<char *>(&secondTagByte), sizeof(secondTagByte)))
        {
            std::cerr << "Error reading second byte of tag 3F." << std::endl;
        }
        tag.push_back(secondTagByte);
    }

    // Read Length
    if (firstTagByte != 0x1E)
    {
        unsigned char lengthByte;
        if (!data->read(reinterpret_cast<char *>(&lengthByte), sizeof(lengthByte)))
        {
            std::cerr << "Error reading length from file." << std::endl;
        }
        length.push_back(lengthByte);
    }
    else
    {
        // Get word length
        unsigned char wordLength;
        if (!data->read(reinterpret_cast<char *>(&wordLength), sizeof(wordLength)))
        {
            std::cerr << "Error reading length from file." << std::endl;
        }

        // Read length
        int intLength = static_cast<int>(wordLength);
        unsigned char bytes[intLength - 128];
        if (!data->read(reinterpret_cast<char *>(bytes), sizeof(bytes)))
        {
            std::cerr << "Error reading bytes from the file!" << std::endl;
        }

        length.assign(bytes, bytes + sizeof(bytes));
    }

    // Read Contents
    contents.resize(hexVectorToInt(length));
    if (!data->read(reinterpret_cast<char *>(contents.data()), contents.size()))
    {
        std::cerr << "Error reading contents from file." << std::endl;
    }
}

int hexVectorToInt(const std::vector<unsigned char> &hexVector)
{
    int result = 0;
    for (unsigned char byte : hexVector)
    {
        result = (result << 8) | byte;
    }

    return result;
}