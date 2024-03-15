#include "MFERData.h"
#include "DataStack.h"
#include "MFERDataCollection.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <type_traits>
#include <cmath>

using namespace std;

unique_ptr<MFERData> parseMFERData(DataStack *dataStack)
{
    uint8_t byte = dataStack->pop_byte();

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

string MFERData::toHexString(string left) const
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
        stream << "| " << contents.stringify();
    }
    return stream.str();
}

ByteVector MFERData::getContents() const
{
    return contents;
}

string MFERData::toString(Encoding encoding) const
{
    return contents.toString(encoding);
}

ByteOrder BLE::getByteOrder() const
{
    return static_cast<ByteOrder>(contents[0]);
}

Encoding TXC::getEncoding() const
{
    return stringToEncoding(contents.toString());
};

uint8_t WFM::getWaveformType() const
{
    return contents[0];
}

string TIM::getMeasurementTime(ByteOrder byteOrder) const
{
    DataStack dataStack(contents);
    tm timeStruct = {};
    timeStruct.tm_year = dataStack.pop_value<uint16_t>(byteOrder) - 1900;
    timeStruct.tm_mon = dataStack.pop_byte();
    timeStruct.tm_mday = dataStack.pop_byte();
    timeStruct.tm_hour = dataStack.pop_byte();
    timeStruct.tm_min = dataStack.pop_byte();
    timeStruct.tm_sec = dataStack.pop_byte();
    ostringstream stream;
    stream << put_time(&timeStruct, "%Y-%m-%dT%H:%M:%S");
    return stream.str();
}

string AGE::getBirthDate(ByteOrder byteOrder) const
{
    DataStack dataStack(contents);
    uint8_t years = dataStack.pop_byte();
    if (years != 0xFF)
    {
        stringstream stream;
        stream << (int)years << " years, " << (int)dataStack.pop_value<uint16_t>(byteOrder) << " days";
        return stream.str();
    }
    dataStack.pop_front(2);
    uint16_t year = dataStack.pop_value<uint16_t>(byteOrder);
    if (year == 0xffff)
    {
        return "N/A";
    }
    tm timeStruct = {};
    timeStruct.tm_year = year - 1900;
    timeStruct.tm_mon = dataStack.pop_byte();
    timeStruct.tm_mday = dataStack.pop_byte();
    ostringstream stream;
    stream << put_time(&timeStruct, "%Y-%m-%d");
    return stream.str();
}

string SEX::getPatientSex() const
{
    return contents[0] == 0x00   ? "Unknown"
           : contents[0] == 0x01 ? "Male"
           : contents[0] == 0x02 ? "Female"
                                 : "Other";
}

float IVL::getSamplingInterval() const
{
    int base = contents[2];
    int exponent = contents[1] - 256;
    return base * pow(10, exponent);
}

string IVL::getSamplingIntervalString() const
{
    ostringstream stream;
    stream << (int)contents[2] << "x10^" << (int)contents[1] - 256 << " (s)";
    return stream.str();
}

NIBPEvent EVT::getNIBPEvent(ByteOrder byteOrder) const
{
    DataStack dataStack(contents);
    NIBPEvent event;
    event.eventCode = dataStack.pop_value<uint16_t>(byteOrder);
    event.startTime = dataStack.pop_value<uint32_t>(byteOrder);
    event.duration = dataStack.pop_value<uint16_t>(byteOrder);
    event.information = dataStack.pop_front(54).toString();
    return event;
}

ATT::ATT(DataStack *dataStack)
{
    channelIndex = dataStack->pop_byte();
    length = dataStack->pop_byte();
    contents = dataStack->pop_front(length);
    attributes = getAttributes();
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
               << left << data->toHexString(leftStream.str());
    }
    return stream.str();
}

vector<unique_ptr<MFERData>> ATT::getAttributes() const
{
    return parseMFERDataCollection(contents);
}

Channel ATT::getChannel(ByteOrder byteOrder) const
{
    const vector<unique_ptr<MFERData>> channelAttributes = getAttributes();
    Channel channel;
    for (const auto &attribute : channelAttributes)
    {
        if (LDN *ldn = dynamic_cast<LDN *>(attribute.get()))
        {
            channel.leadInfo = ldn->getLeadInfo(byteOrder);
        }
        else if (DTP *dtp = dynamic_cast<DTP *>(attribute.get()))
        {
            channel.dataType = dtp->getDataType();
        }
        else if (BLK *blk = dynamic_cast<BLK *>(attribute.get()))
        {
            channel.blockLength = blk->getContents().toInt<uint32_t>(byteOrder);
        }
        else if (IVL *ivl = dynamic_cast<IVL *>(attribute.get()))
        {
            channel.samplingInterval = ivl->getSamplingInterval();
            channel.samplingIntervalString = ivl->getSamplingIntervalString();
        }
        else if (SEN *sen = dynamic_cast<SEN *>(attribute.get()))
        {
            channel.samplingResolution = sen->getSamplingResolution();
        }
    }
    return channel;
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

LeadInfo LDN::getLeadInfo(ByteOrder byteOrder) const
{
    return LeadMap.at(static_cast<Lead>(contents.toInt<uint16_t>(byteOrder)));
}

DataType DTP::getDataType() const
{
    return static_cast<DataType>(contents[0]);
}

float SEN::getSamplingResolution() const
{
    DataStack dataStack(contents);
    dataStack.pop_front();
    int exponent = 256 - (int)dataStack.pop_byte();
    int base = dataStack.pop_bytes<uint16_t>(2);
    return base * pow(10, exponent);
}