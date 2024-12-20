#ifndef NIHONKOHDENDATA_H
#define NIHONKOHDENDATA_H

#include "MFERDataCollection.h"
#include "MFERData.h"
#include "FileManager.h"
#include "ByteVector.h"
#include <variant>
#include <cstdint>
#include <vector>

struct Header
{
    std::string preamble;
    ByteOrder byteOrder = ByteOrder::ENDIAN_LITTLE;
    std::string modelInfo;
    uint8_t waveformType; // Unsure of encoding
    std::string measurementTimeISO;
    std::string patientID;
    std::string patientName;
    std::string birthDateISO;
    std::string patientSex;
    float samplingInterval;
    std::string samplingIntervalString;
    std::vector<NIBPEvent> events;
    uint16_t sequenceCount;
    uint8_t channelCount;
    std::vector<Channel> channels;
    std::string toString() const;
};

struct Interval
{
    double start = 0;
    double end = 0;
};

class NihonKohdenData
{
public:
    NihonKohdenData(ByteVector dataVector);
    NihonKohdenData(const std::string &fileName) : NihonKohdenData(FileManager::readBinaryFile(fileName)){};

    inline Header getHeader() const { return collectDataFields(collection.getMFERDataVector()); }
    void anonymize();
    void setChannelSelection(int index, bool active);
    void setIntervalSelection(double start = 0, double end = 0);

    void printHexData() const;
    void printHeader() const;
    void writeToBinary(const std::string &fileName) const;
    void writeToCsv(const std::string &fileName) const;

private:
    MFERDataCollection collection;
    std::vector<bool> channelSelection = std::vector<bool>(17, true);
    Interval intervalSelection; // Interval selection in seconds

    Header collectDataFields(const std::vector<std::unique_ptr<MFERData>> &mferDataVector) const;
};

#endif
