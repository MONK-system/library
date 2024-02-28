#ifndef NIHONKOHDENDATA_H
#define NIHONKOHDENDATA_H

#include <variant>
#include <cstdint>
#include <vector>
#include "MFERDataCollection.h"
#include "MFERData.h"
#include "FileManager.h"
#include "HexVector.h"

enum class PatientSex
{
    UNKNOWN,
    MALE,
    FEMALE,
    OTHER
};

struct NIBPData // TODO: elaborate on time format.
{
    int eventCode;
    std::string startTime; // TODO: Implement time format
    int duration;
    int sys;
    int dias;
    int mean;
    int PR;
};

// Lead (Tag of content of MWF_LDN) codes, in order of documentation.
// Comments describe their meaning and integer value.
enum class Lead : unsigned short // 2 bytes
{
    STATUS = 0x1040,     // 4160 Status: When status information includes body position
    BODY_POS = 0x1041,   // 4161 Body position: When body position is output as a continuous waveform
    BODY_MVMNT = 0x1042, // 4162 Body movement
    RESP = 0x1043,       // 4163 Respiration: When the respiration measurment method is not specified. Including snoring etc.
    IRW = 0x00A0,        // 160 Impedance respiration waveform
    BP = 0x008F,         // 143 Blood pressure: When NIBP or another blood pressure measurement is not specified
    SPO2 = 0x00AF,       // 175 SpO₂
    ECG1 = 0x1046,       // 4166 ECG1
    ECG2 = 0x1047,       // 4167 ECG2
    ECG3 = 0x1048,       // 4168 ECG3
    ECG4 = 0x1049,       // 4169 ECG4: Used when the lead name is not clear
};

// Data Type (Tag of content of MWF_DTP field) codes in order of documentation.
// (OF NOTE, "16 bit signed integer 0 to 65535" has been named Int_16_U, aka "unsigned")
enum class DataType : unsigned char // 1 byte
{
    INT_16_S = 0x00,  // 16 bit signed integer -32768 to 32767
    INT_16_U = 0x01,  // 16 bit signed integer 0 to 65535 (documented as signed)
    INT_32_S = 0x02,  // 32 bit signed integer
    INT_8_U = 0x03,   // 8 bit unsigned integer
    STATUS_16 = 0x04, // 16 bit status field
    INT_8_S = 0x05,   // 8 bit signed integer
    INT_32_U = 0x06,  // 32 bit unsigned integer
    FLOAT_32 = 0x07,  // 32 bit single precision floating point number
    FLOAT_64 = 0x08,  // 64 bit single precision floating point number
    AHA_8 = 0x09,     // 8 bit AHA compression
};

std::vector<double> popChannelData(DataStack &waveformDataStack, uint64_t num, DataType dataType, ByteOrder byteOrder = ByteOrder::ENDIAN_BIG);

struct Channel
{
    Lead waveformAttributes;
    DataType dataType;
    int blockLength;
    float samplingInterval;
    float samplingResolution;
    std::vector<double> data;
};

class NihonKohdenData
{
public:
    NihonKohdenData(std::vector<unsigned char> dataVector);
    NihonKohdenData(const std::string &fileName) : NihonKohdenData(FileManager::readBinaryFile(fileName)){};

    void printData() const;
    void printDataFields() const;
    void writeWaveformToFile(const std::string &fileName, int channelIndex) const;

    struct DataFields
    {
        std::wstring preamble;
        ByteOrder byteOrder = ByteOrder::ENDIAN_LITTLE;
        EncodedString modelInfo;
        unsigned short longwaveformType; // Unsure of encoding
        std::string time;                // TODO: Implement time format
        EncodedString patientID;
        EncodedString patientName;
        std::string birthDate; // TODO: Implement birth date / age format
        PatientSex patientSex;
        float samplingInterval;
        std::vector<NIBPData> events;
        int sequenceCount;
        int channelCount;
        std::vector<Channel> channels;
    };

private:
    MFERDataCollection collection;
    DataFields fields;

    DataFields collectDataFields(const std::vector<std::unique_ptr<MFERData>> &mferDataVector);
};

#endif