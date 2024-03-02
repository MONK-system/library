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

using namespace std;

int MFERData::maxByteLength = 100;

unique_ptr<MFERData> parseMFERData(DataStack *dataStack)
{
    unsigned char byte = dataStack->pop_byte();

    switch (byte)
    {
    case PRE::tag:
        return make_unique<PRE>(dataStack);
    case BLE::tag:
        return make_unique<BLE>(dataStack);
    case TXC::tag:
        return make_unique<TXC>(dataStack);
    case MAN::tag:
        return make_unique<MAN>(dataStack);
    case WFM::tag:
        return make_unique<WFM>(dataStack);
    case TIM::tag:
        return make_unique<TIM>(dataStack);
    case PID::tag:
        return make_unique<PID>(dataStack);
    case PNM::tag:
        return make_unique<PNM>(dataStack);
    case AGE::tag:
        return make_unique<AGE>(dataStack);
    case SEX::tag:
        return make_unique<SEX>(dataStack);
    case IVL::tag:
        return make_unique<IVL>(dataStack);
    case EVT::tag:
        return make_unique<EVT>(dataStack);
    case SEQ::tag:
        return make_unique<SEQ>(dataStack);
    case CHN::tag:
        return make_unique<CHN>(dataStack);
    case NUL::tag:
        return make_unique<NUL>(dataStack);
    case ATT::tag:
        return make_unique<ATT>(dataStack);
    case WAV::tag:
        return make_unique<WAV>(dataStack);
    case END::tag:
        return make_unique<END>(dataStack);
    case LDN::tag:
        return make_unique<LDN>(dataStack);
    case DTP::tag:
        return make_unique<DTP>(dataStack);
    case BLK::tag:
        return make_unique<BLK>(dataStack);
    case SEN::tag:
        return make_unique<SEN>(dataStack);
    default:
        throw invalid_argument("Invalid tag (int): " + (int)byte);
    }
}

MFERData::MFERData(DataStack *dataStack)
{
    length = dataStack->pop_byte();
    contents = dataStack->pop_front(length);
}

string MFERData::toString(string left) const
{
    string tagStr = tagString();
    string lengthStr = lengthString();
    string contentsStr = contentsString(left);

    ostringstream output;
    output << left << "| " << setw(5) << setfill(' ') << tagStr << " | " << setw(11) << setfill(' ') << lengthStr << " " << contentsStr;
    return output.str();
}

string MFERData::contentsString(string left) const
{
    ostringstream stream;
    if (contents.size() > (size_t)maxByteLength)
    {
        stream << string("| ...");
    }
    else
    {
        stream << "| " << stringifyBytes(contents);
    }
    return stream.str();
}

vector<unsigned char> MFERData::getContents() const
{
    return contents;
}

EncodedString MFERData::getEncodedString(Encoding encoding) const
{
    return convertToEncodedString(contents, encoding);
}

Encoding MFERData::_getEncoding() const
{
    throw runtime_error("No encoding for this data type.");
}

float MFERData::_getSamplingInterval() const
{
    throw runtime_error("No sampling interval for this data type.");
}

string MFERData::_getSamplingIntervalString() const
{
    throw runtime_error("No sampling interval for this data type.");
}

float MFERData::_getSamplingResolution() const
{
    throw runtime_error("No sampling resolution for this data type.");
}

vector<unique_ptr<MFERData>> MFERData::_getAttributes() const
{
    throw runtime_error("No attributes for this data type.");
}

Encoding TXC::_getEncoding() const
{
    return stringToEncoding(convertToString(contents));
};

float IVL::_getSamplingInterval() const
{
    int base = contents[2];
    int exponent = contents[1] - 256;
    return base * pow(10, exponent);
}

string IVL::_getSamplingIntervalString() const
{
    ostringstream stream;
    stream << (int)contents[2] << "×10^" << (int)contents[1] - 256 << " (s)";
    return stream.str();
}

ATT::ATT(DataStack *dataStack)
{
    channel = dataStack->pop_byte();
    length = dataStack->pop_byte();
    contents = dataStack->pop_front(length);
    attributes = parseMFERDataCollection(contents);
}

vector<unique_ptr<MFERData>> ATT::_getAttributes() const
{
    vector<unique_ptr<MFERData>> collection = parseMFERDataCollection(contents);
    return collection;
}

string ATT::contentsString(string left) const
{
    ostringstream stream;
    stream << headerString() << "\n"
           << left << spacerString() << sectionString();
    ostringstream leftStream;
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
    return base * pow(10, exponent);
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

string END::contentsString(string left) const
{
    return "| End of file.";
}