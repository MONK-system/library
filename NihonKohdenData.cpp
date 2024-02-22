#include "NihonKohdenData.h"
#include "MFERData.h"
#include "HexVector.h"
#include "DataStack.h"
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
                auto values = waveformDataStack.pop_values<long long>(channel.blockLength * fields.sequenceCount, getDataTypeSize(channel.dataType));
                channel.data.insert(channel.data.end(), values.begin(), values.end());
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
    std::wcout << L"Byte Order: " << (fields.byteOrder == ByteOrder::LITTLE_ENDIAN ? L"Little Endian" : L"Big Endian") << std::endl;
    std::wcout << L"Model Info: " << fields.modelInfo.str << std::endl;
    std::wcout << L"Longwaveform Type: " << fields.longwaveformType << std::endl;
    std::wcout << L"Patient ID: " << fields.patientID.str << std::endl;
    std::wcout << L"Patient Name: " << fields.patientName.str << std::endl;
    std::wcout << L"Sampling Interval: " << fields.samplingInterval << std::endl;
    std::wcout << L"Sequence Count: " << fields.sequenceCount << std::endl;
    std::wcout << L"Channel Count: " << fields.channelCount << std::endl;
}