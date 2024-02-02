
// Byte tag enums, in order of documentation
enum class MWF : unsigned short // Typically 1 byte. Channels can have assigned byte
{
    PRE = 0x40, // Preamble
    BLE = 0x01, // Byte order
    TXC = 0x0A, // Character code
    MAN = 0x17, // Model information
    WFM = 0x08, // Type of waveform
    TIM = 0x85, // Measurement time
    PID = 0x82, // Patient ID
    // TXC
    PNM = 0x81, // Patient name
    // TXC
    AGE = 0x83, // Patient age
    SEX = 0x84, // Patient sex/gender
    IVL = 0x0B, // Sampling interval
    EVT = 0x41, // Event (NIBP data)
    // More EVT
    SEQ = 0x06, // Number of sequences
    CHN = 0x05, // Number of channels
    NUL = 0x12, // NULL value (0x8000)

    ATT = 0x3F, // A channel, next byte in file specifies number
    // (Documentation specifies 0x00 is Channel 1, 0x10 is 16 and 0x11 is 17? File starts 0x00 and upwards)
    ATT00 = 0x3F00, // Channel 1
    ATT01 = 0x3F01, // Channel 2
    ATT02 = 0x3F02, // Channel 3
    ATT03 = 0x3F03, // Channel 4
    ATT04 = 0x3F04, // Channel 5
    ATT05 = 0x3F05, // Channel 6
    ATT06 = 0x3F06, // Channel 7
    ATT07 = 0x3F07, // Channel 8
    ATT08 = 0x3F08, // Channel 9
    ATT09 = 0x3F09, // Channel 10
    ATT0A = 0x3F0A, // Channel 11
    ATT0B = 0x3F0B, // Channel 12
    ATT0C = 0x3F0C, // Channel 13
    ATT0D = 0x3F0D, // Channel 14
    ATT0E = 0x3F0E, // Channel 15
    ATT0F = 0x3F0F, // Channel 16
    ATT10 = 0x3F10, // Channel 17
    ATT11 = 0x3F11, // Channel 17?

    WAV = 0x1E, // Waveform data
    END = 0x80, // End of file

    // Channel attributes:
    LDN = 0x09, // Waveform attributes (Contains Lead)
    DTP = 0x0A, // Data type
    BLK = 0x04, // Data block length
    // IVL
    SEN = 0x0C, // Sampling resolution
};

// Lead (Tag of content of MWF_LDN) codes, in order of documentation.
// Comments describe their meaning and integer value.
enum class LDN : unsigned short // 2 bytes
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

// Data Type (Tag of content of MWF_DTP) codes in order of documentation.
// (OF NOTE, "16 bit signed integer 0 to 65535" has been named Int16U, aka "unsigned")
enum class DTP : unsigned char // 1 byte
{
    INT16S = 0x00,   // 16 bit signed integer -32768 to 32767
    INT16U = 0x01,   // 16 bit signed integer 0 to 65535 (documented as signed)
    INT32S = 0x02,   // 32 bit signed integer
    INT8U = 0x03,    // 8 bit unsigned integer
    STATUS16 = 0x04, // 16 bit status field
    INT8S = 0x05,    // 8 bit signed integer
    INT32U = 0x06,   // 32 bit unsigned integer
    FLOAT32 = 0x07,  // 32 bit single precision floating point number
    FLOAT64 = 0x08,  // 64 bit single precision floating point number
    AHA8 = 0x09,     // 8 bit AHA compression
};