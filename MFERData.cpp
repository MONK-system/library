#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <type_traits>
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
    return "| Tag   | Length      | Contents \n"
           "|-------|-------------|------------>";
}

std::string MFERData::toString(int maxByteLength) const
{
    std::string tagString = stringifyBytes(intToHexVector<decltype(tag)>(tag));
    std::string lengthString = stringifyBytes(intToHexVector<decltype(length)>(length));
    std::string contentsString = stringifyBytes(contents);

    if (contentsString.size() > maxByteLength)
    {
        contentsString = std::string("...");
    }
    if (length == 0x00) {
        contentsString = std::string("End of file.");
    }

    std::ostringstream output;
    output << "| " << std::setw(5) << std::setfill(' ') << tagString << " | " << std::setw(11) << std::setfill(' ') << lengthString << " | " << contentsString;
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
            throw std::runtime_error("Error reading tag from file, end of file.");
        }
    }

    if (firstTagByte == 0x80)
    {
        setTag(firstTagByte);
        setLength(0x00);
        return;
    }

    if (firstTagByte == 0x00)
    {
        throw std::runtime_error("Encountered invalid tag (00), stopping read.");
    }

    // Special case for tag 3F: read next byte as part of the tag
    if (firstTagByte == 0x3F)
    {
        unsigned char secondTagByte;
        if (!data->read(reinterpret_cast<char *>(&secondTagByte), sizeof(secondTagByte)))
        {
            std::cerr << "Error reading second byte of tag 3F." << std::endl;
        }
        setTag((firstTagByte << 8) + secondTagByte);
    }
    else
    {
        setTag(firstTagByte);
    }

    // Read Length
    if (firstTagByte != 0x1E)
    {
        unsigned char lengthByte;
        if (!data->read(reinterpret_cast<char *>(&lengthByte), sizeof(lengthByte)))
        {
            std::cerr << "Error reading length from file." << std::endl;
        }
        setLength(lengthByte);
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
        std::vector<unsigned char> bytes(wordLength - 128);
        if (!data->read(reinterpret_cast<char *>(bytes.data()), bytes.size()))
        {
            std::cerr << "Error reading bytes from the file!" << std::endl;
        }

        setLength(hexVectorToInt<unsigned long long>(bytes));
    }

    // Read Contents
    contents.resize(length);
    if (!data->read(reinterpret_cast<char *>(contents.data()), contents.size()))
    {
        std::cerr << "Error reading contents from file." << std::endl;
    }
}