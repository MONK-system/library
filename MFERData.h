#ifndef MFERDATA_H
#define MFERDATA_H

#include "DataStack.h"
#include "ByteVector.h"
#include <memory>
#include <cstdint>

using namespace std;

class MFERData
{
public:
    MFERData() = default;
    MFERData(DataStack *dataStack);

    inline static uint64_t maxByteLength = 100;

    static inline string headerString() { return "| Tag   | Length      | Contents"; }
    static inline string sectionString() { return "|-------|-------------|----------->"; }
    static inline string spacerString() { return "|       |             "; }
    string toString(string left) const;

    EncodedString getEncodedString(Encoding encoding) const;

    virtual uint8_t getTag() const { return 0x00; };
    const uint64_t &getLength() const { return length; };
    ByteVector getContents() const;

    inline Encoding getEncoding() const { return _getEncoding(); }
    inline float getSamplingInterval() const { return _getSamplingInterval(); }
    inline string getSamplingIntervalString() const { return _getSamplingIntervalString(); }
    inline float getSamplingResolution() const { return _getSamplingResolution(); }
    inline vector<unique_ptr<MFERData>> getAttributes() const { return _getAttributes(); }

protected:
    uint64_t length;
    ByteVector contents;

    inline string tagString() const { return ByteVector::fromInt<decltype(getTag())>(getTag()).stringify(); };
    inline string lengthString() const { return ByteVector::fromInt<decltype(length)>(length).stringify(); };
    virtual string contentsString(string left) const;

    virtual Encoding _getEncoding() const;
    virtual float _getSamplingInterval() const;
    virtual string _getSamplingIntervalString() const;
    virtual float _getSamplingResolution() const;
    virtual vector<unique_ptr<MFERData>> _getAttributes() const;
};

unique_ptr<MFERData> parseMFERData(DataStack *dataStack);

class PRE : public MFERData // Preamble
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x40;
    uint8_t getTag() const { return tag; }
};

class BLE : public MFERData // Byte order
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x01;
    uint8_t getTag() const { return tag; }
};

class TXC : public MFERData // Character code
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x03;
    uint8_t getTag() const { return tag; }

public:
    Encoding _getEncoding() const;
};

class MAN : public MFERData // Model information
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x17;
    uint8_t getTag() const { return tag; }
};

class WFM : public MFERData // Type of waveform
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x08;
    uint8_t getTag() const { return tag; }
};

class TIM : public MFERData // Measurement time
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x85;
    uint8_t getTag() const { return tag; }
};

class PID : public MFERData // Patient ID
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x82;
    uint8_t getTag() const { return tag; }
};

class PNM : public MFERData // Patient name
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x81;
    uint8_t getTag() const { return tag; }
};

class AGE : public MFERData // Patient age
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x83;
    uint8_t getTag() const { return tag; }
};

class SEX : public MFERData // Patient sex/gender
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x84;
    uint8_t getTag() const { return tag; }
};

class IVL : public MFERData // Sampling interval
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x0B;
    uint8_t getTag() const { return tag; }
    float _getSamplingInterval() const;
    string _getSamplingIntervalString() const;
};

class EVT : public MFERData // Event (NIBP data)
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x41;
    uint8_t getTag() const { return tag; }
};

class SEQ : public MFERData // Number of sequences
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x06;
    uint8_t getTag() const { return tag; }
};

class CHN : public MFERData // Number of channels
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x05;
    uint8_t getTag() const { return tag; }
};

class NUL : public MFERData // NULL value (0x8000)
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x12;
    uint8_t getTag() const { return tag; }
};

class ATT : public MFERData // A channel, next byte in file specifies number
{
public:
    static const uint8_t tag = 0x3F;
    uint8_t getTag() const { return tag; }
    ATT(DataStack *dataStack);

    vector<unique_ptr<MFERData>> _getAttributes() const;

private:
    uint8_t channel;
    vector<unique_ptr<MFERData>> attributes;
    string contentsString(string left) const;
};

class WAV : public MFERData // Waveform data
{
public:
    static const uint8_t tag = 0x1E;
    uint8_t getTag() const { return tag; }
    WAV(DataStack *dataStack);

private:
    uint8_t wordLength;
};

class END : public MFERData // End of file
{
public:
    static const uint8_t tag = 0x80;
    uint8_t getTag() const { return tag; }
    END(DataStack *dataStack);

private:
    string contentsString(string left) const;
};

class LDN : public MFERData // Waveform attributes (Contains Lead)
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x09;
    uint8_t getTag() const { return tag; }
};

class DTP : public MFERData // Data type
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x0A;
    uint8_t getTag() const { return tag; }
};

class BLK : public MFERData // Data block length
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x04;
    uint8_t getTag() const { return tag; }
};

class SEN : public MFERData // Sampling resolution
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x0C;
    uint8_t getTag() const { return tag; }
    float _getSamplingResolution() const;
};

#endif // MFERDATA_H
