#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <type_traits>
#include "MFERData.h"
#include "HexVector.h"
#include "DataStack.h"
#include "MFERDataCollection.h"

MFERData::MFERData(DataStack *dataStack)
{
    try
    {
        tag = dataStack->pop_byte();
        parseData(dataStack);
        if (tag >= 0x3F00 && tag <= 0x3F11)
        {
            dataCollection = parseMFERDataCollection(contents);
        }
    }
    catch (const std::runtime_error &e)
    {
        throw std::runtime_error("Constructor failed: " + std::string(e.what()));
    }
}

void MFERData::parseData(DataStack *dataStack)
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
        setTag((tag << 8) + dataStack->pop_byte());
    }

    // Read Length
    if (tag != 0x1E)
    {
        setLength(dataStack->pop_byte());
    }
    else
    {
        // Get word length
        unsigned char wordLength = dataStack->pop_byte() - 128;

        // Read length
        setLength(dataStack->pop_hex(wordLength));
    }

    // Read Contents
    contents = dataStack->pop_front(length);
}
std::string MFERData::toString(int maxByteLength, std::string left) const
{
    std::string tagString = stringifyBytes(intToHexVector<decltype(tag)>(tag));
    std::string lengthString = stringifyBytes(intToHexVector<decltype(length)>(length));
    std::ostringstream contentsStream;

    if (dataCollection.size() > 0)
    {
        std::ostringstream contentLeft;
        contentLeft << left << spacerString();
        contentsStream << contentsString(dataCollection, maxByteLength, contentLeft.str());
    }
    else
    {
        if (length == 0x00)
        {
            contentsStream << std::string("| End of file.");
        }
        else if (contents.size() > maxByteLength)
        {
            contentsStream << std::string("| ...");
        }
        else
        {
            contentsStream << "| " << stringifyBytes(contents);
        }
    }

    std::ostringstream output;
    output << left << "| " << std::setw(5) << std::setfill(' ') << tagString << " | " << std::setw(11) << std::setfill(' ') << lengthString << " " << contentsStream.str();
    return output.str();
}