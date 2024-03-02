#ifndef MFERDATA_H
#define MFERDATA_H

#include <memory>
#include "DataStack.h"
#include "MFERDataCollection.h"
#include "ByteVector.h"

using namespace std;

class MFERDataCollection;

class MFERData
{
public:
    MFERData() = default;
    MFERData(DataStack *dataStack);

    static int maxByteLength;

    static inline string headerString() { return "| Tag   | Length      | Contents"; }
    static inline string sectionString() { return "|-------|-------------|----------->"; }
    static inline string spacerString() { return "|       |             "; }
    string toString(string left) const;

    EncodedString getEncodedString(Encoding encoding) const;

    virtual unsigned char getTag() const { return 0x00; };
    const unsigned long long &getLength() const { return length; };
    ByteVector getContents() const;

    template <typename T>
    const void setLength(T num)
    {
        static_assert(is_integral<T>::value, "Function only accepts integral types.");
        length = static_cast<decltype(length)>(num);
    }

    inline Encoding getEncoding() const { return _getEncoding(); }
    inline float getSamplingInterval() const { return _getSamplingInterval(); }
    inline string getSamplingIntervalString() const { return _getSamplingIntervalString(); }
    inline float getSamplingResolution() const { return _getSamplingResolution(); }
    inline vector<unique_ptr<MFERData>> getAttributes() const { return _getAttributes(); }

protected:
    unsigned long long length;
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
    static const unsigned char tag = 0x40;
    unsigned char getTag() const { return tag; }
};

class BLE : public MFERData // Byte order
{
public:
    using MFERData::MFERData;
    static const unsigned char tag = 0x01;
    unsigned char getTag() const { return tag; }
};

class TXC : public MFERData // Character code
{
public:
    using MFERData::MFERData;
    static const unsigned char tag = 0x03;
    unsigned char getTag() const { return tag; }

public:
    Encoding _getEncoding() const;
};

class MAN : public MFERData // Model information
{
public:
    using MFERData::MFERData;
    static const unsigned char tag = 0x17;
    unsigned char getTag() const { return tag; }
};

class WFM : public MFERData // Type of waveform
{
public:
    using MFERData::MFERData;
    static const unsigned char tag = 0x08;
    unsigned char getTag() const { return tag; }
};

class TIM : public MFERData // Measurement time
{
public:
    using MFERData::MFERData;
    static const unsigned char tag = 0x85;
    unsigned char getTag() const { return tag; }
};

class PID : public MFERData // Patient ID
{
public:
    using MFERData::MFERData;
    static const unsigned char tag = 0x82;
    unsigned char getTag() const { return tag; }
};

class PNM : public MFERData // Patient name
{
public:
    using MFERData::MFERData;
    static const unsigned char tag = 0x81;
    unsigned char getTag() const { return tag; }
};

class AGE : public MFERData // Patient age
{
public:
    using MFERData::MFERData;
    static const unsigned char tag = 0x83;
    unsigned char getTag() const { return tag; }
};

class SEX : public MFERData // Patient sex/gender
{
public:
    using MFERData::MFERData;
    static const unsigned char tag = 0x84;
    unsigned char getTag() const { return tag; }
};

class IVL : public MFERData // Sampling interval
{
public:
    using MFERData::MFERData;
    static const unsigned char tag = 0x0B;
    unsigned char getTag() const { return tag; }
    float _getSamplingInterval() const;
    string _getSamplingIntervalString() const;
};

class EVT : public MFERData // Event (NIBP data)
{
public:
    using MFERData::MFERData;
    static const unsigned char tag = 0x41;
    unsigned char getTag() const { return tag; }
};

class SEQ : public MFERData // Number of sequences
{
public:
    using MFERData::MFERData;
    static const unsigned char tag = 0x06;
    unsigned char getTag() const { return tag; }
};

class CHN : public MFERData // Number of channels
{
public:
    using MFERData::MFERData;
    static const unsigned char tag = 0x05;
    unsigned char getTag() const { return tag; }
};

class NUL : public MFERData // NULL value (0x8000)
{
public:
    using MFERData::MFERData;
    static const unsigned char tag = 0x12;
    unsigned char getTag() const { return tag; }
};

class ATT : public MFERData // A channel, next byte in file specifies number
{
public:
    static const unsigned char tag = 0x3F;
    unsigned char getTag() const { return tag; }
    ATT(DataStack *dataStack);

    vector<unique_ptr<MFERData>> _getAttributes() const;

private:
    unsigned char channel;
    vector<unique_ptr<MFERData>> attributes;
    string contentsString(string left) const;
};

class WAV : public MFERData // Waveform data
{
public:
    static const unsigned char tag = 0x1E;
    unsigned char getTag() const { return tag; }
    WAV(DataStack *dataStack);

private:
    unsigned char wordLength;
};

class END : public MFERData // End of file
{
public:
    static const unsigned char tag = 0x80;
    unsigned char getTag() const { return tag; }
    END(DataStack *dataStack);

private:
    string contentsString(string left) const;
};

class LDN : public MFERData // Waveform attributes (Contains Lead)
{
public:
    using MFERData::MFERData;
    static const unsigned char tag = 0x09;
    unsigned char getTag() const { return tag; }
};

class DTP : public MFERData // Data type
{
public:
    using MFERData::MFERData;
    static const unsigned char tag = 0x0A;
    unsigned char getTag() const { return tag; }
};

class BLK : public MFERData // Data block length
{
public:
    using MFERData::MFERData;
    static const unsigned char tag = 0x04;
    unsigned char getTag() const { return tag; }
};

class SEN : public MFERData // Sampling resolution
{
public:
    using MFERData::MFERData;
    static const unsigned char tag = 0x0C;
    unsigned char getTag() const { return tag; }
    float _getSamplingResolution() const;
};

#endif // MFERDATA_H
