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

vector<double> popChannelData(DataStack &waveformDataStack, uint64_t num, DataType dataType); // Forward declaration

Header NihonKohdenData::collectDataFields(const vector<unique_ptr<MFERData>> &mferDataVector)
{
    Header header;

    Encoding encoding = Encoding::UTF8;

    for (const auto &data : mferDataVector)
    {
        if (PRE *pre = dynamic_cast<PRE *>(data.get()))
        {
            header.preamble = pre->toString();
        }
        else if (BLE *ble = dynamic_cast<BLE *>(data.get()))
        {
            header.byteOrder = ble->getByteOrder();
        }
        else if (TXC *txc = dynamic_cast<TXC *>(data.get()))
        {
            encoding = txc->getEncoding();
        }
        else if (MAN *man = dynamic_cast<MAN *>(data.get()))
        {
            header.modelInfo = man->toString(encoding);
        }
        else if (WFM *wfm = dynamic_cast<WFM *>(data.get()))
        {
            header.waveformType = wfm->getWaveformType();
        }
        else if (TIM *tim = dynamic_cast<TIM *>(data.get()))
        {
            header.measurementTimeISO = tim->getMeasurementTime(header.byteOrder);
        }
        else if (PID *pid = dynamic_cast<PID *>(data.get()))
        {
            header.patientID = pid->toString(encoding);
        }
        else if (PNM *pnm = dynamic_cast<PNM *>(data.get()))
        {
            header.patientName = pnm->toString(encoding);
        }
        else if (AGE *age = dynamic_cast<AGE *>(data.get()))
        {
            header.birthDateISO = age->getBirthDate(header.byteOrder);
        }
        else if (SEX *sex = dynamic_cast<SEX *>(data.get()))
        {
            header.patientSex = sex->getPatientSex();
        }
        else if (IVL *ivl = dynamic_cast<IVL *>(data.get()))
        {
            header.samplingInterval = ivl->getSamplingInterval();
            header.samplingIntervalString = ivl->getSamplingIntervalString();
        }
        else if (EVT *evt = dynamic_cast<EVT *>(data.get()))
        {
            header.events.push_back(evt->getNIBPEvent(header.byteOrder));
        }
        else if (SEQ *seq = dynamic_cast<SEQ *>(data.get()))
        {
            header.sequenceCount = seq->getContents().toInt<uint16_t>(header.byteOrder);
        }
        else if (CHN *chn = dynamic_cast<CHN *>(data.get()))
        {
            header.channelCount = chn->getContents()[0];
        }
        else if (data->getTag() == NUL::tag)
        {
            // null value
        }
        else if (ATT *att = dynamic_cast<ATT *>(data.get()))
        {
            header.channels.push_back(att->getChannel(header.byteOrder));
        }
        else if (WAV *wav = dynamic_cast<WAV *>(data.get()))
        {
            DataStack waveformDataStack(wav->getContents());
            for (auto &channel : header.channels)
            {
                channel.data = popChannelData(waveformDataStack, channel.blockLength * header.sequenceCount, channel.dataType);
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
    return header;
}

void NihonKohdenData::printData() const
{
    cout << endl
         << collection.toString() << endl;
}

void NihonKohdenData::printHeader() const
{
    cout << "\nData Fields: \n";
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
    stringstream channelsStream;
    uint32_t largestBlockLength = 0;
    double samplingInterval = header.samplingInterval;
    for (auto channel : header.channels)
    {
        channelsStream << ", " << channel.leadInfo.attribute << ": " << channel.leadInfo.samplingResolution;
        if (channel.blockLength > largestBlockLength)
        {
            largestBlockLength = channel.blockLength;
            samplingInterval = channel.samplingInterval;
        }
    }
    stringstream headerStream;
    headerStream << "Time: "
                 << "(s)" << channelsStream.str();

    file.writeLine(headerStream.str()); // Write header to file

    // Write waveform data
    vector<string> lines;
    uint64_t totalBlocks = header.sequenceCount * largestBlockLength;
    lines.reserve(totalBlocks);

    int channelIntervals[header.channels.size()];
    for (size_t i = 0; i < header.channels.size(); i++)
    {
        channelIntervals[i] = largestBlockLength / header.channels[i].blockLength;
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
        for (size_t j = 0; j < header.channels.size(); j++)
        {
            if (channelIntervals[j] == 1)
            {
                line << ", " << header.channels[j].data[i];
            }
            else
            {
                // If i * (channel.blockLength/largestBlocklength) is an integer, then write the value
                if (i % channelIntervals[j] == 0)
                {
                    line << ", " << header.channels[j].data[i / channelIntervals[j]];
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

    cout << "Complete." << endl;
}

vector<double> popChannelData(DataStack &waveformDataStack, uint64_t num, DataType dataType)
{
    switch (dataType)
    {
    case DataType::INT_16_S:
        return waveformDataStack.pop_doubles<int16_t>(num);
    case DataType::INT_16_U:
        return waveformDataStack.pop_doubles<uint16_t>(num);
    case DataType::INT_32_S:
        return waveformDataStack.pop_doubles<int32_t>(num);
    case DataType::INT_8_U:
        return waveformDataStack.pop_doubles<uint8_t>(num);
    case DataType::STATUS_16:
        return waveformDataStack.pop_doubles<uint16_t>(num);
    case DataType::INT_8_S:
        return waveformDataStack.pop_doubles<int8_t>(num);
    case DataType::INT_32_U:
        return waveformDataStack.pop_doubles<uint32_t>(num);
    case DataType::FLOAT_32:
        return waveformDataStack.pop_doubles<float>(num);
    case DataType::FLOAT_64:
        return waveformDataStack.pop_doubles<double>(num);
    case DataType::AHA_8:
        return waveformDataStack.pop_doubles<uint8_t>(num);
    default:
        throw runtime_error("Invalid data type");
    }
}