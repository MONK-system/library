#ifndef MFERDATA_H
#define MFERDATA_H

#include "DataStack.h"
#include "ByteVector.h"

#include <memory>
#include <cstdint>
#include <map>

class MFERData
{
public:
    MFERData() = default;
    MFERData(DataStack *dataStack);

    inline static uint64_t maxByteLength = 100;

    static inline std::string headerString() { return "| Tag   | Length      | Contents"; }
    static inline std::string sectionString() { return "|-------|-------------|----------->"; }
    static inline std::string spacerString() { return "|       |             "; }
    std::string toHexString(std::string left) const;

    std::string toString(Encoding encoding = Encoding::ASCII) const;

    virtual uint8_t getTag() const { return 0x00; };
    const uint64_t &getLength() const { return length; };
    ByteVector getContents() const;
    virtual ByteVector toByteVector() const;

    virtual void anonymize(){};

protected:
    uint64_t length;
    ByteVector contents;

    inline std::string tagString() const { return ByteVector::fromInt<decltype(getTag())>(getTag()).stringify(); };
    inline std::string lengthString() const { return ByteVector::fromInt<decltype(length)>(length).stringify(); };
    virtual std::string contentsString(std::string left) const;
};

std::unique_ptr<MFERData> parseMFERData(DataStack *dataStack);

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
    ByteOrder getByteOrder() const;
};

class TXC : public MFERData // Character code
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x03;
    uint8_t getTag() const { return tag; }
    Encoding getEncoding() const;
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
    uint8_t getWaveformType() const; // Unsure of encoding
};

class TIM : public MFERData // Measurement time
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x85;
    uint8_t getTag() const { return tag; }
    std::string getMeasurementTime(ByteOrder byteOrder) const; // Unsure of full encoding (ms?)
};

class PID : public MFERData // Patient ID
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x82;
    uint8_t getTag() const { return tag; }
    void anonymize();
};

class PNM : public MFERData // Patient name
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x81;
    uint8_t getTag() const { return tag; }
    void anonymize();
};

class AGE : public MFERData // Patient age
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x83;
    uint8_t getTag() const { return tag; }
    std::string getBirthDate(ByteOrder byteOrder) const;
    void anonymize();
};

class SEX : public MFERData // Patient sex/gender
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x84;
    uint8_t getTag() const { return tag; }
    std::string getPatientSex() const;
    void anonymize();
};

class IVL : public MFERData // Sampling interval
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x0B;
    uint8_t getTag() const { return tag; }
    float getSamplingInterval() const;
    std::string getSamplingIntervalString() const;
};

struct NIBPEvent
{
    uint16_t eventCode;
    uint32_t startTime; // Offset from measurement start time?
    uint32_t duration;
    std::string information;
};

class EVT : public MFERData // Event (NIBP data)
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x41;
    uint8_t getTag() const { return tag; }
    NIBPEvent getNIBPEvent(ByteOrder byteOrder) const;
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

// Data Type (Tag of content of MWF_DTP field) codes in order of documentation.
// (OF NOTE, "16 bit signed integer 0 to 65535" has been named Int_16_U, aka "unsigned")
enum class DataType : uint8_t // 1 byte
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

// Lead (Tag of content of MWF_LDN) codes, in order of documentation.
// Comments describe their meaning and integer value.
enum class Lead : uint16_t // 2 bytes
{
    // Pacing marks
    PACING_STATUS = 0x1040,     // 4160 Status: When status information includes body position
    PACING_BODY_POS = 0x1041,   // 4161 Body position: When body position is output as a continuous waveform
    PACING_BODY_MVMNT = 0x1042, // 4162 Body movement
    PACING_RESP = 0x1043,       // 4163 Respiration: When the respiration measurment method is not specified. Including snoring etc.
    PACING_IRW = 0x00A0,        // 160 Impedance respiration waveform
    PACING_BP = 0x008F,         // 143 Blood pressure: When NIBP or another blood pressure measurement is not specified
    PACING_SPO2 = 0x00AF,       // 175 SpO₂
    PACING_ECG1 = 0x1046,       // 4166 ECG1
    PACING_ECG2 = 0x1047,       // 4167 ECG2
    PACING_ECG3 = 0x1048,       // 4168 ECG3
    PACING_ECG4 = 0x1049,       // 4169 ECG4: Used when the lead name is not clear

    // Waveform information leads
    // ECG leads
    ECG_I = 0x0001,       // ECG I (Lead I)
    ECG_II = 0x0002,      // ECG II (Lead II)
    ECG_III = 0x003D,     // ECG III (Lead III)
    ECG_AVR = 0x003E,     // ECG AVR (Augmented vector right)
    ECG_AVL = 0x003F,     // ECG AVL (Augmented vector left)
    ECG_AVF = 0x0040,     // ECG AVF (Augmented vector foot)
    ECG_V1 = 0x0003,      // ECG V1
    ECG_V2 = 0x0004,      // ECG V2
    ECG_V3 = 0x0005,      // ECG V3
    ECG_V4 = 0x0006,      // ECG V4
    ECG_V5 = 0x0007,      // ECG V5
    ECG_V6 = 0x0008,      // ECG V6
    ECG_V = 0xC000,       // ECG V (Vector)
    ECG_MCL = 0x005B,     // ECG MCL (Modified chest lead)
    ECG_ECG1 = 0x0001,    // ECG ECG1
    ECG_ECG2 = 0x0002,    // ECG ECG2
    ECG_TRACE_1 = 0xC003, // ECG Trace 1
    ECG_TRACE_2 = 0xC004, // ECG Trace 2

    // Other leads
    RESP = 0xC005,       // Respiration
    RESP_IMP = 0xC006,   // Impedance respiration
    RESP_THERM = 0xC007, // Thermistor respiration
    SPO2 = 0xC008,       // SpO₂ (SpO₂ 1)
    SPO2_2 = 0xC009,     // SpO₂-2 (SpO₂ 2)
    ART = 0xC00A,        // ART (Arterial pressure)
    ART_2 = 0xC00B,      // ART-2 (Arterial pressure 2)
    RAD = 0xC00C,        // RAD
    DORS = 0xC00D,       // DORS (Doralis pedis artery pressure?)
    AO = 0xC00E,         // AO (Aortic pressure)
    FEM = 0xC00F,        // FEM (Femoral artery pressure)
    UA = 0xC010,         // UA (Ulnar artery pressure)
    UV = 0xC011,         // UV (Umbilical vein pressure)
    PAP = 0xC012,        // PAP (Pulmonary artery pressure)
    CVP = 0xC013,        // CVP (Central venous pressure)
    RAP = 0xC014,        // RAP (Right atrial pressure)
    RVP = 0xC015,        // RVP (Right ventricular pressure)
    LAP = 0xC016,        // LAP (Left atrial pressure)
    LVP = 0xC017,        // LVP (Left ventricular pressure)
    ICP = 0xC018,        // ICP (Intracranial pressure)
    ICP_2 = 0xC019,      // ICP-2 (Intracranial pressure 2)
    ICP_3 = 0xC01A,      // ICP-3 (Intracranial pressure 3)
    ICP_4 = 0xC01B,      // ICP-4 (Intracranial pressure 4)
    PRESS = 0xC01C,      // PRESS (Pressure)
    PRESS_2 = 0xC01D,    // PRESS-2 (Pressure 2)
    PRESS_3 = 0xC01E,    // PRESS-3 (Pressure 3)
    PRESS_4 = 0xC01F,    // PRESS-4 (Pressure 4)
    PRESS_5 = 0xC020,    // PRESS-5 (Pressure 5)
    PRESS_6 = 0xC021,    // PRESS-6 (Pressure 6)
    PRESS_7 = 0xC022,    // PRESS-7 (Pressure 7)
    PRESS_8 = 0xC023,    // PRESS-8 (Pressure 8)
    CO2 = 0xC024,        // CO₂ (Carbon dioxide)
    FIO2 = 0xC025,       // FiO₂ (Fraction of inspired oxygen)
    FLOW_1 = 0xC026,     // FLOW (Respiration flow 1)
    FLOW_2 = 0xC027,     // FLOW (Respiration flow 2)
    PAW = 0xC028,        // PAW (Airway pressure)
    VENT_1 = 0xC029,     // VENT (Ventilator 1) (mmHg)
    VENT_2 = 0xC02A,     // VENT (Ventilator 2) (L)
    VENT_3 = 0xC02B,     // VENT (Ventilator 3) (cmH₂O)
    ANES_1 = 0xC02C,     // ANES (Anesthesia 1) (mmHg)
    ANES_2 = 0xC02D,     // ANES (Anesthesia 2) (%)
    ANES_3 = 0xC02E,     // ANES (Anesthesia 3) (%)
    ANES_4 = 0xC02F,     // ANES (Anesthesia 4) (%)
    ANES_5 = 0xC030,     // ANES (Anesthesia 5) (L)
    ANES_6 = 0xC031,     // ANES (Anesthesia 6) (cmH₂O)
    ANES_7 = 0xC032,     // ANES (Anesthesia 7) (%)
    BIS = 0xC033,        // BIS (Bispectral index)
    EXT_9000_1 = 0xC034, // EXT 9000 (External 9000 1)
    EXT_9000_2 = 0xC035, // EXT 9000 (External 9000 2)
    EXT_9000_3 = 0xC036, // EXT 9000 (External 9000 3)
    EXT_9000_4 = 0xC037, // EXT 9000 (External 9000 4)
    EEG = 0xC038,        // EEG (Electroencephalogram)
    EEG_2 = 0xC039,      // EEG-2 (Electroencephalogram 2)
    PICCO = 0xC03A,      // PICCO (Pulse contour cardiac output)
    EEG_3 = 0xC03B,      // EEG-3 (Electroencephalogram 3)
    EEG_4 = 0xC03C,      // EEG-4 (Electroencephalogram 4)
    EEG_5 = 0xC03D,      // EEG-5 (Electroencephalogram 5)
    EEG_6 = 0xC03E,      // EEG-6 (Electroencephalogram 6)
    EEG_7 = 0xC03F,      // EEG-7 (Electroencephalogram 7)
    EEG_8 = 0xC040,      // EEG-8 (Electroencephalogram 8)
};

// Struct for holding lead information
struct LeadInfo
{
    Lead lead;
    std::string attribute;
    std::string samplingResolution;
    uint8_t unitCode;
};

struct Channel
{
    LeadInfo leadInfo;
    DataType dataType;
    uint32_t blockLength;
    float samplingInterval;
    std::string samplingIntervalString;
    float samplingResolution;
    std::vector<double> data;
};

class ATT : public MFERData // A channel, next byte in file specifies number
{
public:
    static const uint8_t tag = 0x3F;
    uint8_t getTag() const { return tag; }
    ATT(DataStack *dataStack);
    std::vector<std::unique_ptr<MFERData>> getAttributes() const;
    Channel getChannel(ByteOrder byteOrder) const;
    virtual ByteVector toByteVector() const;

private:
    uint8_t channelIndex;
    std::vector<std::unique_ptr<MFERData>> attributes;
    std::string contentsString(std::string left) const;
};

class WAV : public MFERData // Waveform data
{
public:
    static const uint8_t tag = 0x1E;
    uint8_t getTag() const { return tag; }
    WAV(DataStack *dataStack);
    virtual ByteVector toByteVector() const;

private:
    uint8_t wordLength;
    ByteVector lengthBytes;
};

class END : public MFERData // End of file
{
public:
    static const uint8_t tag = 0x80;
    uint8_t getTag() const { return tag; }
    END(DataStack *dataStack);

private:
    std::string contentsString(std::string left) const;
};

class LDN : public MFERData // Waveform attributes (Contains Lead)
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x09;
    uint8_t getTag() const { return tag; }
    LeadInfo getLeadInfo(ByteOrder ByteOrder) const;
};

class DTP : public MFERData // Data type
{
public:
    using MFERData::MFERData;
    static const uint8_t tag = 0x0A;
    uint8_t getTag() const { return tag; }
    DataType getDataType() const;
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
    float getSamplingResolution() const;
};

// Map of Lead codes to their string representation
const std::map<Lead, LeadInfo>
    LeadMap = {
        {Lead::PACING_STATUS, LeadInfo{Lead::PACING_STATUS, "Pacing Status", "N/A", 0}},
        {Lead::PACING_BODY_POS, LeadInfo{Lead::PACING_BODY_POS, "Pacing Body Position", "N/A", 0}},
        {Lead::PACING_BODY_MVMNT, LeadInfo{Lead::PACING_BODY_MVMNT, "Pacing Body Movement", "N/A", 0}},
        {Lead::PACING_RESP, LeadInfo{Lead::PACING_RESP, "Pacing Respiration", "N/A", 0}},
        {Lead::PACING_IRW, LeadInfo{Lead::PACING_IRW, "Pacing Impedance Respiration Waveform", "N/A", 0}},
        {Lead::PACING_BP, LeadInfo{Lead::PACING_BP, "Pacing Blood Pressure", "N/A", 0}},
        {Lead::PACING_SPO2, LeadInfo{Lead::PACING_SPO2, "Pacing SpO2", "N/A", 0}},
        {Lead::PACING_ECG1, LeadInfo{Lead::PACING_ECG1, "Pacing ECG1", "N/A", 0}},
        {Lead::PACING_ECG2, LeadInfo{Lead::PACING_ECG2, "Pacing ECG2", "N/A", 0}},
        {Lead::PACING_ECG3, LeadInfo{Lead::PACING_ECG3, "Pacing ECG3", "N/A", 0}},
        {Lead::PACING_ECG4, LeadInfo{Lead::PACING_ECG4, "Pacing ECG4", "N/A", 0}},

        // Waveform information
        // Unit codes: 0 = V, 1 = mmHg, 3 = cmH₂O 7 = %, 11 = Ω, 19 = L
        {Lead::ECG_I, LeadInfo{Lead::ECG_I, "ECG I", "2x10^-6 (V)", 0}},
        {Lead::ECG_II, LeadInfo{Lead::ECG_II, "ECG II", "2x10^-6 (V)", 0}},
        {Lead::ECG_III, LeadInfo{Lead::ECG_III, "ECG III", "2x10^-6 (V)", 0}},
        {Lead::ECG_AVR, LeadInfo{Lead::ECG_AVR, "ECG AVR", "2x10^-6 (V)", 0}},
        {Lead::ECG_AVL, LeadInfo{Lead::ECG_AVL, "ECG AVL", "2x10^-6 (V)", 0}},
        {Lead::ECG_AVF, LeadInfo{Lead::ECG_AVF, "ECG AVF", "2x10^-6 (V)", 0}},
        {Lead::ECG_V1, LeadInfo{Lead::ECG_V1, "ECG V1", "2x10^-6 (V)", 0}},
        {Lead::ECG_V2, LeadInfo{Lead::ECG_V2, "ECG V2", "2x10^-6 (V)", 0}},
        {Lead::ECG_V3, LeadInfo{Lead::ECG_V3, "ECG V3", "2x10^-6 (V)", 0}},
        {Lead::ECG_V4, LeadInfo{Lead::ECG_V4, "ECG V4", "2x10^-6 (V)", 0}},
        {Lead::ECG_V5, LeadInfo{Lead::ECG_V5, "ECG V5", "2x10^-6 (V)", 0}},
        {Lead::ECG_V6, LeadInfo{Lead::ECG_V6, "ECG V6", "2x10^-6 (V)", 0}},
        {Lead::ECG_V, LeadInfo{Lead::ECG_V, "ECG V", "2x10^-6 (V)", 0}},
        {Lead::ECG_MCL, LeadInfo{Lead::ECG_MCL, "ECG MCL", "2x10^-6 (V)", 0}},
        {Lead::ECG_ECG1, LeadInfo{Lead::ECG_ECG1, "ECG ECG1", "2x10^-6 (V)", 0}},
        {Lead::ECG_ECG2, LeadInfo{Lead::ECG_ECG2, "ECG ECG2", "2x10^-6 (V)", 0}},
        {Lead::ECG_TRACE_1, LeadInfo{Lead::ECG_TRACE_1, "ECG Trace 1", "2x10^-6 (V)", 0}},
        {Lead::ECG_TRACE_2, LeadInfo{Lead::ECG_TRACE_2, "ECG Trace 2", "2x10^-6 (V)", 0}},

        // Other leads
        {Lead::RESP, LeadInfo{Lead::RESP, "Respiration", "25x10^-4 (Ω)", 11}},
        {Lead::RESP_IMP, LeadInfo{Lead::RESP_IMP, "Impedance Respiration", "25x10^-4 (Ω)", 11}},
        {Lead::RESP_THERM, LeadInfo{Lead::RESP_THERM, "Thermistor Respiration", "25x10^-4 (Ω)", 11}},
        {Lead::SPO2, LeadInfo{Lead::SPO2, "SpO₂", "6105x10^-7 (%)", 7}},
        {Lead::SPO2_2, LeadInfo{Lead::SPO2_2, "SpO₂-2", "6105x10^-7 (%)", 7}},
        {Lead::ART, LeadInfo{Lead::ART, "ART", "125x10^-3 (mmHg)", 1}},
        {Lead::ART_2, LeadInfo{Lead::ART_2, "ART-2", "125x10^-3 (mmHg)", 1}},
        {Lead::RAD, LeadInfo{Lead::RAD, "RAD", "125x10^-3 (mmHg)", 1}},
        {Lead::DORS, LeadInfo{Lead::DORS, "DORS", "125x10^-3 (mmHg)", 1}},
        {Lead::AO, LeadInfo{Lead::AO, "AO", "125x10^-3 (mmHg)", 1}},
        {Lead::FEM, LeadInfo{Lead::FEM, "FEM", "125x10^-3 (mmHg)", 1}},
        {Lead::UA, LeadInfo{Lead::UA, "UA", "125x10^-3 (mmHg)", 1}},
        {Lead::UV, LeadInfo{Lead::UV, "UV", "125x10^-3 (mmHg)", 1}},
        {Lead::PAP, LeadInfo{Lead::PAP, "PAP", "125x10^-3 (mmHg)", 1}},
        {Lead::CVP, LeadInfo{Lead::CVP, "CVP", "125x10^-3 (mmHg)", 1}},
        {Lead::RAP, LeadInfo{Lead::RAP, "RAP", "125x10^-3 (mmHg)", 1}},
        {Lead::RVP, LeadInfo{Lead::RVP, "RVP", "125x10^-3 (mmHg)", 1}},
        {Lead::LAP, LeadInfo{Lead::LAP, "LAP", "125x10^-3 (mmHg)", 1}},
        {Lead::LVP, LeadInfo{Lead::LVP, "LVP", "125x10^-3 (mmHg)", 1}},

        {Lead::ICP, LeadInfo{Lead::ICP, "ICP", "125x10^-3 (mmHg)", 1}},
        {Lead::ICP_2, LeadInfo{Lead::ICP_2, "ICP-2", "125x10^-3 (mmHg)", 1}},
        {Lead::ICP_3, LeadInfo{Lead::ICP_3, "ICP-3", "125x10^-3 (mmHg)", 1}},
        {Lead::ICP_4, LeadInfo{Lead::ICP_4, "ICP-4", "125x10^-3 (mmHg)", 1}},

        {Lead::PRESS, LeadInfo{Lead::PRESS, "Press", "125x10^-3 (mmHg)", 1}},
        {Lead::PRESS_2, LeadInfo{Lead::PRESS_2, "Press-2", "125x10^-3 (mmHg)", 1}},
        {Lead::PRESS_3, LeadInfo{Lead::PRESS_3, "Press-3", "125x10^-3 (mmHg)", 1}},
        {Lead::PRESS_4, LeadInfo{Lead::PRESS_4, "Press-4", "125x10^-3 (mmHg)", 1}},
        {Lead::PRESS_5, LeadInfo{Lead::PRESS_5, "Press-5", "125x10^-3 (mmHg)", 1}},
        {Lead::PRESS_6, LeadInfo{Lead::PRESS_6, "Press-6", "125x10^-3 (mmHg)", 1}},
        {Lead::PRESS_7, LeadInfo{Lead::PRESS_7, "Press-7", "125x10^-3 (mmHg)", 1}},
        {Lead::PRESS_8, LeadInfo{Lead::PRESS_8, "Press-8", "125x10^-3 (mmHg)", 1}},

        {Lead::CO2, LeadInfo{Lead::CO2, "CO₂", "1x10^-1 (mmHg)", 1}},
        {Lead::FIO2, LeadInfo{Lead::FIO2, "FiO₂", "1x10^-2 (%)", 7}},

        {Lead::FLOW_1, LeadInfo{Lead::FLOW_1, "Flow", "1x10^-3 (L)", 19}},
        {Lead::FLOW_2, LeadInfo{Lead::FLOW_2, "Flow", "1x10^-3 (L)", 19}},

        {Lead::PAW, LeadInfo{Lead::PAW, "PAW", "1x10^-2 (cmH₂O)", 3}},

        {Lead::VENT_1, LeadInfo{Lead::VENT_1, "Ventilator", "1x10^-1 (mmHg)", 1}},
        {Lead::VENT_2, LeadInfo{Lead::VENT_2, "Ventilator", "1x10^-3 (L)", 19}},
        {Lead::VENT_3, LeadInfo{Lead::VENT_3, "Ventilator", "1x10^-2 (cmH₂O)", 3}},

        {Lead::ANES_1, LeadInfo{Lead::ANES_1, "Anesthesia", "1x10^-1 (mmHg)", 1}},
        {Lead::ANES_2, LeadInfo{Lead::ANES_2, "Anesthesia", "1x10^-2 (%)", 7}},
        {Lead::ANES_3, LeadInfo{Lead::ANES_3, "Anesthesia", "1x10^-0 (%)", 7}},
        {Lead::ANES_4, LeadInfo{Lead::ANES_4, "Anesthesia", "1x10^-2 (%)", 7}},
        {Lead::ANES_5, LeadInfo{Lead::ANES_5, "Anesthesia", "1x10^-3 (L)", 19}},
        {Lead::ANES_6, LeadInfo{Lead::ANES_6, "Anesthesia", "1x10^-2 (cmH₂O)", 3}},
        {Lead::ANES_7, LeadInfo{Lead::ANES_7, "Anesthesia", "1x10^-2 (%)", 7}},

        {Lead::BIS, LeadInfo{Lead::BIS, "BIS", "1x10^-7 (V)", 0}},

        {Lead::EXT_9000_1, LeadInfo{Lead::EXT_9000_1, "External-9000", "3125x10^-7 (V)", 0}},
        {Lead::EXT_9000_2, LeadInfo{Lead::EXT_9000_2, "External-9000", "3125x10^-7 (V)", 0}},
        {Lead::EXT_9000_3, LeadInfo{Lead::EXT_9000_3, "External-9000", "3125x10^-7 (V)", 0}},
        {Lead::EXT_9000_4, LeadInfo{Lead::EXT_9000_4, "External-9000", "3125x10^-7 (V)", 0}},

        {Lead::EEG, LeadInfo{Lead::EEG, "EEG", "125x10^-9 (V)", 0}},
        {Lead::EEG_2, LeadInfo{Lead::EEG_2, "EEG-2", "125x10^-9 (V)", 0}},
        {Lead::PICCO, LeadInfo{Lead::PICCO, "PiCCO", "1x10^-2 (mmHg)", 1}},
        {Lead::EEG_3, LeadInfo{Lead::EEG_3, "EEG-3", "125x10^-9 (V)", 0}},
        {Lead::EEG_4, LeadInfo{Lead::EEG_4, "EEG-4", "125x10^-9 (V)", 0}},
        {Lead::EEG_5, LeadInfo{Lead::EEG_5, "EEG-5", "125x10^-9 (V)", 0}},
        {Lead::EEG_6, LeadInfo{Lead::EEG_6, "EEG-6", "125x10^-9 (V)", 0}},
        {Lead::EEG_7, LeadInfo{Lead::EEG_7, "EEG-7", "125x10^-9 (V)", 0}},
        {Lead::EEG_8, LeadInfo{Lead::EEG_8, "EEG-8", "125x10^-9 (V)", 0}},
};

#endif // MFERDATA_H
