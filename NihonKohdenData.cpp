#include "NihonKohdenData.h"
#include "MFERData.h"
#include "HexVector.h"
#include "DataStack.h"
#include "FileManager.h"
#include <iostream>
#include <cstdint>

NihonKohdenData::NihonKohdenData(std::vector<unsigned char> dataVector)
{
    collection = MFERDataCollection(dataVector);
    fields = collectDataFields(collection.getMFERDataVector());
}

DataFields NihonKohdenData::collectDataFields(const std::vector<std::unique_ptr<MFERData>> &mferDataVector)
{
    DataFields fields;

    Encoding encoding = Encoding::UTF8;

    for (const auto &data : mferDataVector)
    {
        switch (data->getTag())
        {
        case PRE::tag:
            fields.preamble = data->getEncodedString(Encoding::UTF8).str;
            break;
        case BLE::tag:
            fields.byteOrder = static_cast<ByteOrder>(data->getContents()[0]);
            break;
        case TXC::tag:
            encoding = data->getEncoding();
            break;
        case MAN::tag:
            fields.modelInfo = data->getEncodedString(encoding);
            break;
        case WFM::tag:
            fields.longwaveformType = data->getContents()[0];
            break;
        case TIM::tag:
            fields.time = "Yet to be implemented";
            break;
        case PID::tag:
            fields.patientID = data->getEncodedString(encoding);
            break;
        case PNM::tag:
            fields.patientName = data->getEncodedString(encoding);
            break;
        case AGE::tag:
            fields.birthDate = "Yet to be implemented";
            break;
        case SEX::tag:
            fields.patientSex = static_cast<PatientSex>(data->getContents()[0]);
            break;
        case IVL::tag:
            fields.samplingInterval = data->getSamplingInterval();
            fields.samplingIntervalString = data->getSamplingIntervalString();
            break;
        case EVT::tag:
            // TODO: Implement event data
            break;
        case SEQ::tag:
            fields.sequenceCount = hexVectorToInt<uint16_t>(data->getContents(), fields.byteOrder);
            break;
        case CHN::tag:
            fields.channelCount = (uint8_t)data->getContents()[0];
            break;
        case NUL::tag:
            // null value
            break;
        case ATT::tag:
        {
            const std::vector<std::unique_ptr<MFERData>> channelAttributes = data->getAttributes();
            Channel channel;
            for (const auto &channelAttribute : channelAttributes)
            {
                switch (channelAttribute->getTag())
                {
                case LDN::tag:
                    channel.waveformAttributes = LeadMap.at(static_cast<Lead>(hexVectorToInt<uint16_t>(channelAttribute->getContents(), fields.byteOrder)));
                    break;
                case DTP::tag:
                    channel.dataType = static_cast<DataType>(channelAttribute->getContents()[0]);
                    break;
                case BLK::tag:
                    channel.blockLength = hexVectorToInt<uint32_t>(channelAttribute->getContents(), fields.byteOrder);
                    break;
                case IVL::tag:
                    channel.samplingInterval = channelAttribute->getSamplingInterval();
                    channel.samplingIntervalString = channelAttribute->getSamplingIntervalString();
                    break;
                case SEN::tag:
                    channel.samplingResolution = channelAttribute->getSamplingResolution();
                    break;
                }
            }
            fields.channels.push_back(channel);
            break;
        }
        case WAV::tag:
        {
            DataStack waveformDataStack(data->getContents());
            for (auto &channel : fields.channels)
            {
                channel.data = popChannelData(waveformDataStack, channel.blockLength * fields.sequenceCount, channel.dataType);
            }
            break;
        }
        case END::tag:
            break;
        default:
            break;
        }
    }
    return fields;
}

void NihonKohdenData::printData() const
{
    std::cout << std::endl
              << collection.toString() << std::endl;
}

void NihonKohdenData::printDataFields() const
{
    std::cout << "\nData Fields: \n";
    std::wcout << L"Preamble: " << fields.preamble << std::endl;
    std::wcout << L"Byte Order: " << (fields.byteOrder == ByteOrder::ENDIAN_LITTLE ? L"Little Endian" : L"Big Endian") << std::endl;
    std::wcout << L"Model Info: " << fields.modelInfo.str << std::endl;
    std::wcout << L"Longwaveform Type: " << fields.longwaveformType << std::endl;
    std::wcout << L"Patient ID: " << fields.patientID.str << std::endl;
    std::wcout << L"Patient Name: " << fields.patientName.str << std::endl;
    std::wcout << L"Sampling Interval: " << fields.samplingInterval << std::endl;
    std::wcout << L"Sequence Count: " << fields.sequenceCount << std::endl;
    std::wcout << L"Channel Count: " << fields.channelCount << std::endl;
}

void NihonKohdenData::writeWaveformToCsv(const std::string &fileName) const
{
    std::cout << "\nWriting waveform data to " << fileName << std::endl;
    FileManager file(fileName);

    // Write csv header & get lowest sampling interval
    std::stringstream channelsHeader;
    uint32_t largestBlockLength = 0;
    double samplingInterval = fields.samplingInterval;
    for (auto channel : fields.channels)
    {
        channelsHeader << ", " << channel.waveformAttributes.parameterName << ": " << channel.waveformAttributes.samplingResolution;
        if (channel.blockLength > largestBlockLength)
        {
            largestBlockLength = channel.blockLength;
            samplingInterval = channel.samplingInterval;
        }
    }
    std::stringstream header;
    header << "Time: "
           << "(s)" << channelsHeader.str();

    file.writeLine(header.str()); // Write header to file

    // Write waveform data
    std::vector<std::string> lines;
    uint64_t totalBlocks = fields.sequenceCount * largestBlockLength;
    lines.reserve(totalBlocks);

    int channelIntervals[fields.channels.size()];
    for (std::size_t i = 0; i < fields.channels.size(); i++)
    {
        channelIntervals[i] = largestBlockLength / fields.channels[i].blockLength;
    }

    int loggingInterval = 1000;
    std::cout << "0 / " << totalBlocks << " samples processed";
    for (uint64_t i = 0; i < totalBlocks; i++)
    {
        if (i % loggingInterval == 0)
        {
            std::cout << "\r" << i << " / " << totalBlocks << " samples processed";
        }

        std::stringstream line;
        line << i * samplingInterval;
        for (std::size_t j = 0; j < fields.channels.size(); j++)
        {
            if (channelIntervals[j] == 1)
            {
                line << ", " << fields.channels[j].data[i];
            }
            else
            {
                // If i * (channel.blockLength/largestBlocklength) is an integer, then write the value
                if (i % channelIntervals[j] == 0)
                {
                    line << ", " << fields.channels[j].data[i / channelIntervals[j]];
                }
                else
                {
                    line << ", ";
                }
            }
        }
        lines.push_back(line.str());
    }
    std::cout << "\rProcessing complete. " << totalBlocks << " samples processed.\n";

    file.writeLines(lines);
    file.closeFile();

    std::cout << "Waveform data written to " << fileName << std::endl;
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