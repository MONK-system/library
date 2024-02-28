#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <type_traits>
#include <cmath>
#include "MFERData.h"
#include "HexVector.h"
#include "DataStack.h"
#include "MFERDataCollection.h"

int MFERData::maxByteLength = 100;

std::unique_ptr<MFERData> parseMFERData(DataStack *dataStack)
{
    unsigned char byte = dataStack->pop_byte();

    switch (byte)
    {
    case PRE::tag:
        return std::make_unique<PRE>(dataStack);
    case BLE::tag:
        return std::make_unique<BLE>(dataStack);
    case TXC::tag:
        return std::make_unique<TXC>(dataStack);
    case MAN::tag:
        return std::make_unique<MAN>(dataStack);
    case WFM::tag:
        return std::make_unique<WFM>(dataStack);
    case TIM::tag:
        return std::make_unique<TIM>(dataStack);
    case PID::tag:
        return std::make_unique<PID>(dataStack);
    case PNM::tag:
        return std::make_unique<PNM>(dataStack);
    case AGE::tag:
        return std::make_unique<AGE>(dataStack);
    case SEX::tag:
        return std::make_unique<SEX>(dataStack);
    case IVL::tag:
        return std::make_unique<IVL>(dataStack);
    case EVT::tag:
        return std::make_unique<EVT>(dataStack);
    case SEQ::tag:
        return std::make_unique<SEQ>(dataStack);
    case CHN::tag:
        return std::make_unique<CHN>(dataStack);
    case NUL::tag:
        return std::make_unique<NUL>(dataStack);
    case ATT::tag:
        return std::make_unique<ATT>(dataStack);
    case WAV::tag:
        return std::make_unique<WAV>(dataStack);
    case END::tag:
        return std::make_unique<END>(dataStack);
    case LDN::tag:
        return std::make_unique<LDN>(dataStack);
    case DTP::tag:
        return std::make_unique<DTP>(dataStack);
    case BLK::tag:
        return std::make_unique<BLK>(dataStack);
    case SEN::tag:
        return std::make_unique<SEN>(dataStack);
    default:
        throw std::invalid_argument("Invalid tag (int): " + (int)byte);
    }
}

MFERData::MFERData(DataStack *dataStack)
{
    length = dataStack->pop_byte();
    contents = dataStack->pop_front(length);
}

std::string MFERData::toString(std::string left) const
{
    std::string tagStr = tagString();
    std::string lengthStr = lengthString();
    std::string contentsStr = contentsString(left);

    std::ostringstream output;
    output << left << "| " << std::setw(5) << std::setfill(' ') << tagStr << " | " << std::setw(11) << std::setfill(' ') << lengthStr << " " << contentsStr;
    return output.str();
}

std::string MFERData::contentsString(std::string left) const
{
    std::ostringstream stream;
    if (contents.size() > (size_t)maxByteLength)
    {
        stream << std::string("| ...");
    }
    else
    {
        stream << "| " << stringifyBytes(contents);
    }
    return stream.str();
}

std::vector<unsigned char> MFERData::getContents() const
{
    return contents;
}

EncodedString MFERData::getEncodedString(Encoding encoding) const
{
    return convertToEncodedString(contents, encoding);
}

Encoding MFERData::_getEncoding() const
{
    throw std::runtime_error("No encoding for this data type.");
}

float MFERData::_getSamplingInterval() const
{
    throw std::runtime_error("No sampling interval for this data type.");
}

float MFERData::_getSamplingResolution() const
{
    throw std::runtime_error("No sampling resolution for this data type.");
}

std::vector<std::unique_ptr<MFERData>> MFERData::_getAttributes() const
{
    throw std::runtime_error("No attributes for this data type.");
}

Encoding TXC::_getEncoding() const
{
    return stringToEncoding(convertToString(contents));
};

float IVL::_getSamplingInterval() const
{
    int base = contents[2];
    int exponent = contents[1] - 256;
    return base * std::pow(10, exponent);
}

ATT::ATT(DataStack *dataStack)
{
    channel = dataStack->pop_byte();
    length = dataStack->pop_byte();
    contents = dataStack->pop_front(length);
    attributes = parseMFERDataCollection(contents);
}

std::vector<std::unique_ptr<MFERData>> ATT::_getAttributes() const
{
    std::vector<std::unique_ptr<MFERData>> collection = parseMFERDataCollection(contents);
    return collection;
}

std::string ATT::contentsString(std::string left) const
{
    std::ostringstream stream;
    stream << headerString() << "\n"
           << left << spacerString() << sectionString();
    std::ostringstream leftStream;
    leftStream << left << spacerString();
    for (const auto &data : attributes)
    {
        stream << "\n"
               << left << data->toString(leftStream.str());
    }
    return stream.str();
}

float SEN::_getSamplingResolution() const
{
    DataStack dataStack(contents);
    dataStack.pop_front();
    int exponent = 256 - (int)dataStack.pop_byte();
    int base = dataStack.pop_bytes<unsigned short>(2);
    return base * std::pow(10, exponent);
}

WAV::WAV(DataStack *dataStack)
{
    wordLength = dataStack->pop_byte();
    length = dataStack->pop_bytes<unsigned>(4);
    contents = dataStack->pop_front(length);
}

END::END(DataStack *dataStack)
{
    length = 0x00;
}

std::string END::contentsString(std::string left) const
{
    return "| End of file.";
}