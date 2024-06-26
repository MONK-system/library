#include "NihonKohdenData.h"
#include <iostream>
#include <cstdint>
#include <sstream>
#include <algorithm>

// Forward declaration
std::vector<double> popChannelData(DataStack &waveformDataStack, uint64_t num, DataType dataType, ByteOrder byteOrder);

NihonKohdenData::NihonKohdenData(ByteVector dataVector)
{
    collection = MFERDataCollection(dataVector);
}

Header NihonKohdenData::collectDataFields(const std::vector<std::unique_ptr<MFERData>> &mferDataVector) const
{
    Header fields;

    Encoding encoding = Encoding::UTF8;

    for (const auto &data : mferDataVector)
    {
        if (PRE *pre = dynamic_cast<PRE *>(data.get()))
        {
            fields.preamble = pre->toString();
        }
        else if (BLE *ble = dynamic_cast<BLE *>(data.get()))
        {
            fields.byteOrder = ble->getByteOrder();
        }
        else if (TXC *txc = dynamic_cast<TXC *>(data.get()))
        {
            encoding = txc->getEncoding();
        }
        else if (MAN *man = dynamic_cast<MAN *>(data.get()))
        {
            fields.modelInfo = man->toString(encoding);
        }
        else if (WFM *wfm = dynamic_cast<WFM *>(data.get()))
        {
            fields.waveformType = wfm->getWaveformType();
        }
        else if (TIM *tim = dynamic_cast<TIM *>(data.get()))
        {
            fields.measurementTimeISO = tim->getMeasurementTime(fields.byteOrder);
        }
        else if (PID *pid = dynamic_cast<PID *>(data.get()))
        {
            fields.patientID = pid->toString(encoding);
        }
        else if (PNM *pnm = dynamic_cast<PNM *>(data.get()))
        {
            fields.patientName = pnm->toString(encoding);
        }
        else if (AGE *age = dynamic_cast<AGE *>(data.get()))
        {
            fields.birthDateISO = age->getBirthDate(fields.byteOrder);
        }
        else if (SEX *sex = dynamic_cast<SEX *>(data.get()))
        {
            fields.patientSex = sex->getPatientSex();
        }
        else if (IVL *ivl = dynamic_cast<IVL *>(data.get()))
        {
            fields.samplingInterval = ivl->getSamplingInterval();
            fields.samplingIntervalString = ivl->getSamplingIntervalString();
        }
        else if (EVT *evt = dynamic_cast<EVT *>(data.get()))
        {
            fields.events.push_back(evt->getNIBPEvent(fields.byteOrder));
        }
        else if (SEQ *seq = dynamic_cast<SEQ *>(data.get()))
        {
            fields.sequenceCount = seq->getContents().toInt<uint16_t>(fields.byteOrder);
        }
        else if (CHN *chn = dynamic_cast<CHN *>(data.get()))
        {
            fields.channelCount = chn->getContents()[0];
        }
        else if (data->getTag() == NUL::tag)
        {
            // null value
        }
        else if (ATT *att = dynamic_cast<ATT *>(data.get()))
        {
            fields.channels.push_back(att->getChannel(fields.byteOrder));
        }
        else if (WAV *wav = dynamic_cast<WAV *>(data.get()))
        {
            DataStack waveformDataStack(wav->getContents());
            for (uint16_t i = 0; i < fields.sequenceCount; i++) // For each sequence
            {
                for (auto &channel : fields.channels) // For each channel
                {
                    std::vector<double> sequenceData = popChannelData(waveformDataStack, channel.blockLength, channel.dataType, fields.byteOrder); // Pop the channel data according to the block length and data type size
                    channel.data.insert(channel.data.end(), sequenceData.begin(), sequenceData.end());                                             // Add the sequence data to the channel data
                }
            }
        }
        else if (data->getTag() == END::tag)
        {
            break; // end of file
        }
        else
        {
            std::cerr << "Unknown tag: " << data->getTag() << std::endl;
        }
    }
    return fields;
}

void NihonKohdenData::anonymize()
{
    for (const auto &data : collection.getMFERDataVector())
    {
        data->anonymize();
    }
}

void NihonKohdenData::setChannelSelection(int index, bool active)
{
    if (index < 0 || index >= channelSelection.size())
    {
        throw std::runtime_error("Channel index out of range");
    }
    else
    {
        channelSelection[index] = active;
    }
}

void NihonKohdenData::setIntervalSelection(double start, double end)
{
    if (start < 0)
    {
        throw std::runtime_error("Start interval must be greater than or equal to 0");
    }
    if (end < 0)
    {
        throw std::runtime_error("End interval must be greater than or equal to 0");
    }
    if ((end != 0) && (start > end))
    {
        throw std::runtime_error("Start interval must be 0 or less than or equal to end interval");
    }
    intervalSelection.start = start;
    intervalSelection.end = end;
}

void NihonKohdenData::printHexData() const
{
    std::cout << std::endl
              << collection.toHexString() << std::endl;
}

std::string Header::toString() const
{
    std::stringstream ss;
    ss << "Preamble: " << preamble << std::endl;
    ss << "Byte Order: " << (byteOrder == ByteOrder::ENDIAN_LITTLE ? "Little Endian" : "Big Endian") << std::endl;
    ss << "Model Info: " << modelInfo << std::endl;
    ss << "Measurement Time: " << measurementTimeISO << std::endl;
    ss << "Patient ID: " << patientID << std::endl;
    ss << "Patient Name: " << patientName << std::endl;
    ss << "Birth Date: " << birthDateISO << std::endl;
    ss << "Patient Sex: " << patientSex << std::endl;
    ss << "Sampling Interval: " << samplingIntervalString << std::endl;
    ss << "NIBP Events: " << events.size() << std::endl;
    ss << "Sequence Count: " << (int)sequenceCount << std::endl;
    ss << "Channel Count: " << (int)channelCount << std::endl;
    for (const auto &channel : channels)
    {
        ss << "Channel: " << channel.leadInfo.attribute << std::endl;
        ss << "   Block Length: " << channel.blockLength << std::endl;
        ss << "   Sampling Resolution: " << channel.leadInfo.samplingResolution << std::endl;
    }
    return ss.str();
}

void NihonKohdenData::printHeader() const
{
    Header header = collectDataFields(collection.getMFERDataVector());
    std::cout << "\nHeader: \n";
    std::cout << header.toString() << std::endl;
}

void NihonKohdenData::writeToBinary(const std::string &fileName) const
{
    FileManager::writeBinaryFile(fileName, collection.toByteVector());
}

void NihonKohdenData::writeToCsv(const std::string &fileName) const
{
    Header header = collectDataFields(collection.getMFERDataVector());
    std::cout << "\nWriting waveform data to " << fileName << std::endl;
    FileManager file(fileName);

    // Get channel selection
    std::vector<Channel> outputChannels;
    for (int i = 0; i < header.channels.size(); i++)
    {
        if (channelSelection[i])
        {
            outputChannels.push_back(header.channels[i]);
        }
    }

    // Write csv header & get lowest sampling interval
    std::stringstream channelsHeader;
    uint64_t largestBlockLength = 0;
    double samplingInterval = header.samplingInterval;
    for (auto channel : outputChannels)
    {
        channelsHeader << ", " << channel.leadInfo.attribute << ": " << channel.leadInfo.samplingResolution;
        if (channel.blockLength > largestBlockLength)
        {
            largestBlockLength = channel.blockLength;
            samplingInterval = channel.samplingInterval;
        }
    }
    std::stringstream headerStream;
    headerStream << "Time: "
                 << "(s)" << channelsHeader.str();

    file.writeLine(headerStream.str()); // Write header to file

    // Get interval timestamps
    double start = intervalSelection.start;
    double end = intervalSelection.end;
    if (intervalSelection.end == 0)
    {
        end = header.sequenceCount * largestBlockLength * samplingInterval;
    }

    // Write waveform data
    std::vector<std::string> lines;
    lines.reserve(header.sequenceCount * largestBlockLength);

    std::vector<int> channelIntervals(outputChannels.size());
    for (size_t i = 0; i < outputChannels.size(); i++)
    {
        channelIntervals[i] = largestBlockLength / outputChannels[i].blockLength;
    }

    std::cout << "0 / " << header.sequenceCount << " sequences processed";

    for (uint16_t i = 0; i < header.sequenceCount; i++) // For each sequence
    {
        for (uint64_t j = 0; j < largestBlockLength; j++) // For each block
        {
            double timestamp = (i * largestBlockLength + j) * samplingInterval; // Calculate timestamp
            if (timestamp >= start && timestamp <= end)                         // If timestamp is within interval
            {
                std::stringstream line;                            // Create a new line
                line << timestamp;                                 // Write timestamp
                for (size_t k = 0; k < outputChannels.size(); k++) // For each channel
                {
                    if (channelIntervals[k] == 1) // If the channel has the same block length as the largest block length, then write the value
                    {
                        line << ", " << outputChannels[k].data[i * largestBlockLength + j];
                    }
                    else // If the channel has a different block length, then write the value if the index is a multiple of the interval
                    {
                        if (j % channelIntervals[k] == 0)
                        {
                            line << ", " << outputChannels[k].data[(i * largestBlockLength + j) / channelIntervals[k]];
                        }
                        else
                        {
                            line << ", ";
                        }
                    }
                }
                lines.push_back(line.str());
            }
        }
        std::cout << "\r" << (i + 1) << " / " << header.sequenceCount << " sequences processed";
    }
    std::cout << "\rProcessing complete. " << header.sequenceCount << " sequences processed.\n";

    file.writeLines(lines);
    file.closeFile();

    std::cout << "Complete." << std::endl;
}

std::vector<double> popChannelData(DataStack &waveformDataStack, uint64_t num, DataType dataType, ByteOrder byteOrder)
{
    switch (dataType)
    {
    case DataType::INT_16_S:
        return waveformDataStack.pop_doubles<int16_t>(num, byteOrder);
    case DataType::INT_16_U:
        return waveformDataStack.pop_doubles<uint16_t>(num, byteOrder);
    case DataType::INT_32_S:
        return waveformDataStack.pop_doubles<int32_t>(num, byteOrder);
    case DataType::INT_8_U:
        return waveformDataStack.pop_doubles<uint8_t>(num, byteOrder);
    case DataType::STATUS_16:
        return waveformDataStack.pop_doubles<uint16_t>(num, byteOrder);
    case DataType::INT_8_S:
        return waveformDataStack.pop_doubles<int8_t>(num, byteOrder);
    case DataType::INT_32_U:
        return waveformDataStack.pop_doubles<uint32_t>(num, byteOrder);
    case DataType::FLOAT_32:
        return waveformDataStack.pop_doubles<float>(num, byteOrder);
    case DataType::FLOAT_64:
        return waveformDataStack.pop_doubles<double>(num, byteOrder);
    case DataType::AHA_8:
        return waveformDataStack.pop_doubles<uint8_t>(num, byteOrder);
    default:
        throw std::runtime_error("Invalid data type");
    }
}
