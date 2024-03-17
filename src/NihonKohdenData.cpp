#include "NihonKohdenData.h"
#include <iostream>
#include <cstdint>
#include <sstream>

using namespace std;

NihonKohdenData::NihonKohdenData(ByteVector dataVector)
{
    collection = MFERDataCollection(dataVector);
    header = collectDataFields(collection.getMFERDataVector());
}

Header NihonKohdenData::getHeader() const
{
    return header;
}

vector<double> popChannelData(DataStack &waveformDataStack, uint64_t num, DataType dataType, ByteOrder byteOrder); // Forward declaration

Header NihonKohdenData::collectDataFields(const vector<unique_ptr<MFERData>> &mferDataVector)
{
    Header fields;

    Encoding encoding = Encoding::UTF8;

    for (const auto &data : mferDataVector)
    {
        if (PRE *pre = dynamic_cast<PRE *>(data.get()))
        {
            fields.preamble = pre->getContents().toString();
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
                    vector<double> sequenceData = popChannelData(waveformDataStack, channel.blockLength, channel.dataType, fields.byteOrder); // Pop the channel data according to the block length and data type size
                    channel.data.insert(channel.data.end(), sequenceData.begin(), sequenceData.end());                                        // Add the sequence data to the channel data
                }
            }
        }
        else if (data->getTag() == END::tag)
        {
            break; // end of file
        }
        else
        {
            cerr << "Unknown tag: " << data->getTag() << endl;
        }
    }
    return fields;
}

void NihonKohdenData::printData() const
{
    cout << endl
         << collection.toString() << endl;
}

void NihonKohdenData::printHeader() const
{
    cout << "\nHeader: \n";
    cout << "Preamble: " << header.preamble << endl;
    cout << "Byte Order: " << (header.byteOrder == ByteOrder::ENDIAN_LITTLE ? "Little Endian" : "Big Endian") << endl;
    cout << "Model Info: " << header.modelInfo << endl;
    cout << "Measurement Time: " << header.measurementTimeISO << endl;
    cout << "Patient ID: " << header.patientID << endl;
    cout << "Patient Name: " << header.patientName << endl;
    cout << "Birth Date: " << header.birthDateISO << endl;
    cout << "Patient Sex: " << header.patientSex << endl;
    cout << "Sampling Interval: " << header.samplingIntervalString << endl;
    cout << "NIBP Events: " << header.events.size() << endl;
    cout << "Sequence Count: " << (int)header.sequenceCount << endl;
    cout << "Channel Count: " << (int)header.channelCount << endl;
    for (const auto &channel : header.channels)
    {
        cout << "Channel: " << channel.leadInfo.attribute << endl;
        cout << "   Block Length: " << channel.blockLength << endl;
        cout << "   Sampling Resolution: " << channel.leadInfo.samplingResolution << endl;
    }
}

void NihonKohdenData::writeToCsv(const string &fileName) const
{
    cout << "\nWriting waveform data to " << fileName << endl;
    FileManager file(fileName);

    // Write csv header & get lowest sampling interval
    stringstream channelsHeader;
    uint64_t largestBlockLength = 0;
    double samplingInterval = header.samplingInterval;
    for (auto channel : header.channels)
    {
        channelsHeader << ", " << channel.leadInfo.attribute << ": " << channel.leadInfo.samplingResolution;
        if (channel.blockLength > largestBlockLength)
        {
            largestBlockLength = channel.blockLength;
            samplingInterval = channel.samplingInterval;
        }
    }
    stringstream headerStream;
    headerStream << "Time: "
                 << "(s)" << channelsHeader.str();

    file.writeLine(headerStream.str()); // Write header to file

    // Write waveform data
    vector<string> lines;
    lines.reserve(header.sequenceCount * largestBlockLength);

    vector<int> channelIntervals(header.channels.size());
    for (size_t i = 0; i < header.channels.size(); i++)
    {
        channelIntervals[i] = largestBlockLength / header.channels[i].blockLength;
    }

    cout << "0 / " << header.sequenceCount << " sequences processed";

    for (uint16_t i = 0; i < header.sequenceCount; i++) // For each sequence
    {
        for (uint64_t j = 0; j < largestBlockLength; j++) // For each block
        {
            stringstream line;                                       // Create a new line
            line << (i * largestBlockLength + j) * samplingInterval; // Write timestamp
            for (size_t k = 0; k < header.channels.size(); k++)      // For each channel
            {
                if (channelIntervals[k] == 1) // If the channel has the same block length as the largest block length, then write the value
                {
                    line << ", " << header.channels[k].data[i * largestBlockLength + j];
                }
                else // If the channel has a different block length, then write the value if the index is a multiple of the interval
                {
                    if (j % channelIntervals[k] == 0)
                    {
                        line << ", " << header.channels[k].data[(i * largestBlockLength + j) / channelIntervals[k]];
                    }
                    else
                    {
                        line << ", ";
                    }
                }
            }
            lines.push_back(line.str());
        }
        cout << "\r" << (i + 1) << " / " << header.sequenceCount << " sequences processed";
    }
    cout << "\rProcessing complete. " << header.sequenceCount << " sequences processed.\n";

    file.writeLines(lines);
    file.closeFile();

    cout << "Complete." << endl;
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