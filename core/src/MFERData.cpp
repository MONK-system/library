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

std::unique_ptr<MFERData> parseMFERData(DataStack *dataStack)
{
    try
    {
        uint8_t byte = dataStack->pop_byte();

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
            std::stringstream stream;
            stream << "Invalid tag (int): " << (int)byte;
            throw std::invalid_argument(stream.str());
        }
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error("Error while parsing MFERData: " + std::string(e.what()));
    }
}

MFERData::MFERData(DataStack *dataStack)
{
    length = dataStack->pop_byte();
    contents = dataStack->pop_front(length);
}

std::string MFERData::toHexString(std::string left) const
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
        stream << "| " << contents.stringify();
    }
    return stream.str();
}

ByteVector MFERData::getContents() const
{
    return contents;
}

ByteVector MFERData::toByteVector() const
{
    ByteVector byteVector;
    byteVector.push_back(getTag());
    byteVector.push_back(length);
    byteVector.insert(byteVector.end(), contents.begin(), contents.end());
    return byteVector;
}

std::string MFERData::toString(Encoding encoding) const
{
    return contents.toString(encoding);
}

ByteOrder BLE::getByteOrder() const
{
    return static_cast<ByteOrder>(contents[0]);
}

Encoding TXC::getEncoding() const
{
    std::string encoding = contents.toString();
    if (encoding.find("ASCII") != std::string::npos || encoding.find("ANSI X3.4") != std::string::npos)
    {
        return Encoding::ASCII;
    }
    else if (encoding.find("UTF-8") != std::string::npos)
    {
        return Encoding::UTF8;
    }
    else if (encoding.find("UTF-16LE") != std::string::npos)
    {
        return Encoding::UTF16LE;
    }
    else
    {
        throw std::runtime_error("Unsupported encoding.");
    }
};

uint8_t WFM::getWaveformType() const
{
    return contents[0];
}

std::string TIM::getMeasurementTime(ByteOrder byteOrder) const
{
    DataStack dataStack(contents);
    std::tm timeStruct = {};
    timeStruct.tm_year = dataStack.pop_value<uint16_t>(byteOrder) - 1900;
    timeStruct.tm_mon = dataStack.pop_byte() - 1;
    timeStruct.tm_mday = dataStack.pop_byte();
    timeStruct.tm_hour = dataStack.pop_byte();
    timeStruct.tm_min = dataStack.pop_byte();
    timeStruct.tm_sec = dataStack.pop_byte();
    std::ostringstream stream;
    try
    {
        stream << std::put_time(&timeStruct, "%Y-%m-%dT%H:%M:%S");
    }
    catch (const std::exception &e)
    {
        stream << "Invalid time format.";
    }
    return stream.str();
}

void PID::anonymize()
{
    contents = ByteVector(length, 0x00);
}

void PNM::anonymize()
{
    contents = ByteVector(length, 0x00);
}

std::string AGE::getBirthDate(ByteOrder byteOrder) const
{
    DataStack dataStack(contents);
    uint8_t years = dataStack.pop_byte();
    if (years != 0xFF)
    {
        std::stringstream stream;
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
    timeStruct.tm_mon = dataStack.pop_byte() - 1;
    timeStruct.tm_mday = dataStack.pop_byte();
    std::ostringstream stream;
    try
    {
        stream << std::put_time(&timeStruct, "%Y-%m-%d");
    }
    catch (const std::exception &e)
    {
        stream << "Invalid time format.";
    }
    return stream.str();
}

void AGE::anonymize()
{
    contents = ByteVector(length, 0xFF);
}

std::string SEX::getPatientSex() const
{
    return contents[0] == 0x00   ? "Unknown"
           : contents[0] == 0x01 ? "Male"
           : contents[0] == 0x02 ? "Female"
                                 : "Other";
}

void SEX::anonymize()
{
    contents = ByteVector(length, 0x00);
}

float IVL::getSamplingInterval() const
{
    int base = contents[2];
    int exponent = contents[1] - 256;
    return base * pow(10, exponent);
}

std::string IVL::getSamplingIntervalString() const
{
    std::ostringstream stream;
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
               << left << data->toHexString(leftStream.str());
    }
    return stream.str();
}

std::vector<std::unique_ptr<MFERData>> ATT::getAttributes() const
{
    return parseMFERDataCollection(contents);
}

Channel ATT::getChannel(ByteOrder byteOrder) const
{
    const std::vector<std::unique_ptr<MFERData>> channelAttributes = getAttributes();
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

ByteVector ATT::toByteVector() const
{
    ByteVector byteVector;
    byteVector.push_back(getTag());
    byteVector.push_back(channelIndex);
    byteVector.push_back(length);
    byteVector.insert(byteVector.end(), contents.begin(), contents.end());
    return byteVector;
}

WAV::WAV(DataStack *dataStack)
{
    wordLength = dataStack->pop_byte();
    lengthBytes = dataStack->pop_front((int)wordLength - 128);
    length = lengthBytes.toInt<uint32_t>();
    contents = dataStack->pop_front(length);
}

ByteVector WAV::toByteVector() const
{
    ByteVector byteVector;
    byteVector.push_back(getTag());
    byteVector.push_back(wordLength);
    byteVector.insert(byteVector.end(), lengthBytes.begin(), lengthBytes.end());
    byteVector.insert(byteVector.end(), contents.begin(), contents.end());
    return byteVector;
}

END::END(DataStack *dataStack)
{
    length = 0x00;
}

ByteVector END::toByteVector() const
{
    ByteVector byteVector;
    byteVector.push_back(getTag());
    return byteVector;
}

std::string END::contentsString(std::string left) const
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
