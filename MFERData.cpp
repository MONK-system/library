#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <type_traits>
#include "MFERData.h"
#include "HexVector.h"
#include "DataBlock.h"

MFERData::MFERData(DataBlock *dataBlock)
{
    try
    {
        tag = dataBlock->pop_byte();
        parseData(dataBlock);
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
    if (length == 0x00)
    {
        contentsString = std::string("End of file.");
    }

    std::ostringstream output;
    output << "| " << std::setw(5) << std::setfill(' ') << tagString << " | " << std::setw(11) << std::setfill(' ') << lengthString << " | " << contentsString;
    return output.str();
}

void MFERData::parseData(DataBlock *dataBlock)
{

    if (tag == 0x80)
    {
        setLength(0x00);
        return;
    }

    if (tag == 0x00)
    {
        throw std::runtime_error("Encountered invalid tag (00), stopping read.");
    }

    // Special case for tag 3F: read next byte as part of the tag
    if (tag == 0x3F)
    {
        setTag((tag << 8) + dataBlock->pop_byte());
    }

    // Read Length
    if (tag != 0x1E)
    {
        setLength(dataBlock->pop_byte());
    }
    else
    {
        // Get word length
        unsigned char wordLength = dataBlock->pop_byte() - 128;

        // Read length
        setLength(dataBlock->pop_hex(wordLength));
    }

    // Read Contents
    contents = dataBlock->pop_front(length);
}