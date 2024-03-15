#include "NihonKohdenData.h"
#include <iostream>
#include <cstdint>
#include <sstream>

using namespace std;

NihonKohdenData::NihonKohdenData(ByteVector dataVector)
{
    collection = MFERDataCollection(dataVector);
    fields = collectDataFields(collection.getMFERDataVector());
}

vector<double> popChannelData(DataStack &waveformDataStack, uint64_t num, DataType dataType, ByteOrder byteOrder); // Forward declaration

DataFields NihonKohdenData::collectDataFields(const vector<unique_ptr<MFERData>> &mferDataVector)
{
    DataFields fields;

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
            fields.modelInfo = man->getContents().toEncodedString(encoding);
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
            fields.patientID = pid->getContents().toEncodedString(encoding);
        }
        else if (PNM *pnm = dynamic_cast<PNM *>(data.get()))
        {
            fields.patientName = pnm->getContents().toEncodedString(encoding);
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

void NihonKohdenData::printDataFields() const
{
    cout << "\nData Fields: \n";
    cout << "Preamble: " << fields.preamble << endl;
    cout << "Byte Order: " << (fields.byteOrder == ByteOrder::ENDIAN_LITTLE ? "Little Endian" : "Big Endian") << endl;
    wcout << L"Model Info: " << fields.modelInfo.str << endl;
    cout << "Measurement Time: " << fields.measurementTimeISO << endl;
    wcout << L"Patient ID: " << fields.patientID.str << endl;
    wcout << L"Patient Name: " << fields.patientName.str << endl;
    cout << "Birth Date: " << fields.birthDateISO << endl;
    cout << "Patient Sex: " << fields.patientSex << endl;
    cout << "Sampling Interval: " << fields.samplingIntervalString << endl;
    cout << "NIBP Events: " << fields.events.size() << endl;
    cout << "Sequence Count: " << (int)fields.sequenceCount << endl;
    cout << "Channel Count: " << (int)fields.channelCount << endl;
    for (const auto &channel : fields.channels)
    {
        cout << "Channel: " << channel.leadInfo.parameterName << endl;
        cout << "   Block Length: " << channel.blockLength << endl;
        cout << "   Sampling Resolution: " << channel.leadInfo.samplingResolution << endl;
    }
}

void NihonKohdenData::writeWaveformToCsv(const string &fileName) const
{
    cout << "\nWriting waveform data to " << fileName << endl;
    FileManager file(fileName);

    // Write csv header & get lowest sampling interval
    stringstream channelsHeader;
    uint64_t largestBlockLength = 0;
    double samplingInterval = fields.samplingInterval;
    for (auto channel : fields.channels)
    {
        channelsHeader << ", " << channel.leadInfo.parameterName << ": " << channel.leadInfo.samplingResolution;
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
    lines.reserve(fields.sequenceCount * largestBlockLength);

    int channelIntervals[fields.channels.size()];
    for (size_t i = 0; i < fields.channels.size(); i++)
    {
        channelIntervals[i] = largestBlockLength / fields.channels[i].blockLength;
    }

    cout << "0 / " << fields.sequenceCount << " sequences processed";

    for (uint16_t i = 0; i < fields.sequenceCount; i++) // For each sequence
    {
        for (uint64_t j = 0; j < largestBlockLength; j++) // For each block
        {
            stringstream line;                                       // Create a new line
            line << (i * largestBlockLength + j) * samplingInterval; // Write timestamp
            for (size_t k = 0; k < fields.channels.size(); k++)      // For each channel
            {
                if (channelIntervals[k] == 1) // If the channel has the same block length as the largest block length, then write the value
                {
                    line << ", " << fields.channels[k].data[i * largestBlockLength + j];
                }
                else // If the channel has a different block length, then write the value if the index is a multiple of the interval
                {
                    if (j % channelIntervals[k] == 0)
                    {
                        line << ", " << fields.channels[k].data[(i * largestBlockLength + j) / channelIntervals[k]];
                    }
                    else
                    {
                        line << ", ";
                    }
                }
            }
            lines.push_back(line.str());
        }
        cout << "\r" << (i + 1) << " / " << fields.sequenceCount << " sequences processed";
    }
    cout << "\rProcessing complete. " << fields.sequenceCount << " sequences processed.\n";

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