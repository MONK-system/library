#include "NihonKohdenData.h"
#include "MFERData.h"
#include "ByteVector.h"
#include "DataStack.h"
#include "FileManager.h"
#include <iostream>
#include <cstdint>
#include <sstream>

using namespace std;

NihonKohdenData::NihonKohdenData(ByteVector dataVector)
{
    collection = MFERDataCollection(dataVector);
    fields = collectDataFields(collection.getMFERDataVector());
}

DataFields NihonKohdenData::collectDataFields(const vector<unique_ptr<MFERData>> &mferDataVector)
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
            fields.sequenceCount = data->getContents().toInt<uint16_t>(fields.byteOrder);
            break;
        case CHN::tag:
            fields.channelCount = (uint8_t)data->getContents()[0];
            break;
        case NUL::tag:
            // null value
            break;
        case ATT::tag:
        {
            const vector<unique_ptr<MFERData>> channelAttributes = data->getAttributes();
            Channel channel;
            for (const auto &channelAttribute : channelAttributes)
            {
                switch (channelAttribute->getTag())
                {
                case LDN::tag:
                    channel.waveformAttributes = LeadMap.at(static_cast<Lead>(channelAttribute->getContents().toInt<uint16_t>(fields.byteOrder)));
                    break;
                case DTP::tag:
                    channel.dataType = static_cast<DataType>(channelAttribute->getContents()[0]);
                    break;
                case BLK::tag:
                    channel.blockLength = channelAttribute->getContents().toInt<uint32_t>(fields.byteOrder);
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
    cout << endl
         << collection.toString() << endl;
}

void NihonKohdenData::printDataFields() const
{
    cout << "\nData Fields: \n";
    wcout << L"Preamble: " << fields.preamble << endl;
    wcout << L"Byte Order: " << (fields.byteOrder == ByteOrder::ENDIAN_LITTLE ? L"Little Endian" : L"Big Endian") << endl;
    wcout << L"Model Info: " << fields.modelInfo.str << endl;
    wcout << L"Longwaveform Type: " << fields.longwaveformType << endl;
    wcout << L"Patient ID: " << fields.patientID.str << endl;
    wcout << L"Patient Name: " << fields.patientName.str << endl;
    wcout << L"Sampling Interval: " << fields.samplingInterval << endl;
    wcout << L"Sequence Count: " << fields.sequenceCount << endl;
    wcout << L"Channel Count: " << fields.channelCount << endl;
}

void NihonKohdenData::writeWaveformToCsv(const string &fileName) const
{
    cout << "\nWriting waveform data to " << fileName << endl;
    FileManager file(fileName);

    // Write csv header & get lowest sampling interval
    stringstream channelsHeader;
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
    stringstream header;
    header << "Time: "
           << "(s)" << channelsHeader.str();

    file.writeLine(header.str()); // Write header to file

    // Write waveform data
    vector<string> lines;
    uint64_t totalBlocks = fields.sequenceCount * largestBlockLength;
    lines.reserve(totalBlocks);

    int channelIntervals[fields.channels.size()];
    for (size_t i = 0; i < fields.channels.size(); i++)
    {
        channelIntervals[i] = largestBlockLength / fields.channels[i].blockLength;
    }

    int loggingInterval = 1000;
    cout << "0 / " << totalBlocks << " samples processed";
    for (uint64_t i = 0; i < totalBlocks; i++)
    {
        if (i % loggingInterval == 0)
        {
            cout << "\r" << i << " / " << totalBlocks << " samples processed";
        }

        stringstream line;
        line << i * samplingInterval;
        for (size_t j = 0; j < fields.channels.size(); j++)
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
    cout << "\rProcessing complete. " << totalBlocks << " samples processed.\n";

    file.writeLines(lines);
    file.closeFile();

    cout << "Waveform data written to " << fileName << endl;
}

vector<double> popChannelData(DataStack &waveformDataStack, uint64_t num, DataType dataType, ByteOrder byteOrder)
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
        throw runtime_error("Invalid data type");
    }
}