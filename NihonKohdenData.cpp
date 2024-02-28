#include "NihonKohdenData.h"
#include "MFERData.h"
#include "HexVector.h"
#include "DataStack.h"
#include "FileManager.h"
#include <iostream>

NihonKohdenData::NihonKohdenData(std::vector<unsigned char> dataVector)
{
    collection = MFERDataCollection(dataVector);
    fields = collectDataFields(collection.getMFERDataVector());
}

NihonKohdenData::DataFields NihonKohdenData::collectDataFields(const std::vector<std::unique_ptr<MFERData>> &mferDataVector)
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
            break;
        case EVT::tag:
            // TODO: Implement event data
            break;
        case SEQ::tag:
            fields.sequenceCount = hexVectorToInt<int>(data->getContents(), fields.byteOrder);
            break;
        case CHN::tag:
            fields.channelCount = (int)data->getContents()[0];
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
                    channel.waveformAttributes = static_cast<Lead>(hexVectorToInt<unsigned short>(channelAttribute->getContents(), fields.byteOrder));
                    break;
                case DTP::tag:
                    channel.dataType = static_cast<DataType>(channelAttribute->getContents()[0]);
                    break;
                case BLK::tag:
                    channel.blockLength = hexVectorToInt<int>(channelAttribute->getContents(), fields.byteOrder);
                    break;
                case IVL::tag:
                    channel.samplingInterval = channelAttribute->getSamplingInterval();
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

void NihonKohdenData::writeWaveformToFile(const std::string &fileName, int channelIndex) const
{
    if (channelIndex < 0 || channelIndex >= fields.channelCount)
    {
        throw std::runtime_error("Invalid channel index");
    }

    FileManager file(fileName);

    std::vector<std::string> lines;
    for (auto val : fields.channels[channelIndex].data)
    {
        lines.push_back(std::to_string(val));
    }
    file.writeLines(lines);
    file.closeFile();
}

std::vector<double> popChannelData(DataStack &waveformDataStack, int num, DataType dataType, ByteOrder byteOrder)
{
    switch (dataType)
    {
    case DataType::INT_16_S:
        return waveformDataStack.pop_doubles<short>(num, byteOrder);
    case DataType::INT_16_U:
        return waveformDataStack.pop_doubles<unsigned short>(num, byteOrder);
    case DataType::INT_32_S:
        return waveformDataStack.pop_doubles<int>(num, byteOrder);
    case DataType::INT_8_U:
        return waveformDataStack.pop_doubles<unsigned char>(num, byteOrder);
    case DataType::STATUS_16:
        return waveformDataStack.pop_doubles<unsigned short>(num, byteOrder);
    case DataType::INT_8_S:
        return waveformDataStack.pop_doubles<char>(num, byteOrder);
    case DataType::INT_32_U:
        return waveformDataStack.pop_doubles<unsigned int>(num, byteOrder);
    case DataType::FLOAT_32:
        return waveformDataStack.pop_doubles<float>(num, byteOrder);
    case DataType::FLOAT_64:
        return waveformDataStack.pop_doubles<double>(num, byteOrder);
    case DataType::AHA_8:
        return waveformDataStack.pop_doubles<unsigned char>(num, byteOrder);
    default:
        throw std::runtime_error("Invalid data type");
    }
}