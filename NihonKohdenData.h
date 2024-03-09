#ifndef NIHONKOHDENDATA_H
#define NIHONKOHDENDATA_H

#include "MFERDataCollection.h"
#include "MFERData.h"
#include "FileManager.h"
#include "ByteVector.h"
#include <variant>
#include <cstdint>
#include <vector>

using namespace std;

struct Header
{
    string preamble;
    ByteOrder byteOrder = ByteOrder::ENDIAN_LITTLE;
    string modelInfo;
    uint8_t waveformType; // Unsure of encoding
    string measurementTimeISO;
    string patientID;
    string patientName;
    string birthDateISO;
    string patientSex;
    float samplingInterval;
    string samplingIntervalString;
    vector<NIBPEvent> events;
    uint16_t sequenceCount;
    uint8_t channelCount;
    vector<Channel> channels;
};

class NihonKohdenData
{
public:
    NihonKohdenData(ByteVector dataVector);
    NihonKohdenData(const string &fileName) : NihonKohdenData(FileManager::readBinaryFile(fileName)){};

    Header getHeader() const;

    void printData() const;
    void printDataFields() const;
    void writeToCsv(const string &fileName) const;

private:
    MFERDataCollection collection;
    Header header;

    Header collectDataFields(const vector<unique_ptr<MFERData>> &mferDataVector);
};

#endif