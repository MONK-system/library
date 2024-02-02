
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

    ATT = 0x3F,     // A channel, next byte in file specifies number (Documentation specifies 0x00 is channel 1, 0x10 is 16 and 0x11 is 17? File starts 0x00 and upwards)
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
    LDN = 0x09, // Waveform attributes (Holds Lead)
    DTP = 0x0A, // Data type
    BLK = 0x04, // Data block length
    // IVL
    SEN = 0x0C, // Sampling resolution
};

// Lead (Describes Waveform Attributes) Codes. Comments describe their meaning and integer value.
enum class LDN : unsigned short // 2 bytes
{

};