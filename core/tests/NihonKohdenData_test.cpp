#include <gtest/gtest.h>
#include "NihonKohdenData.h"
#include "FileManager.h"
#include "ByteVector.h"
#include "MFERDataCollection.h"
#include <algorithm>
#include <sstream>

// Helper function to trim null characters
std::string trimNulls(const std::string &str)
{
    std::string trimmed = str;
    trimmed.erase(std::remove(trimmed.begin(), trimmed.end(), '\0'), trimmed.end());
    return trimmed;
}

// Test for NihonKohdenData initialization with ByteVector
TEST(NihonKohdenDataTest, InitializationWithByteVector)
{
    ByteVector bv = FileManager::readBinaryFile("test-file.MWF");
    NihonKohdenData nkData(bv);

    Header header = nkData.getHeader();
    EXPECT_EQ(trimNulls(header.preamble), "MFR Monitoring Waveform");
    EXPECT_EQ(header.byteOrder, ByteOrder::ENDIAN_LITTLE);
    EXPECT_EQ(trimNulls(header.modelInfo), "NIHON KOHDEN^CNS6000^0, 5, 0, 9");
    EXPECT_EQ(header.measurementTimeISO, "2019-06-19T13:20:00");
    EXPECT_EQ(trimNulls(header.patientID), "12345");
    EXPECT_EQ(trimNulls(header.patientName), "TRWRU");
    EXPECT_EQ(header.birthDateISO, "N/A");
    EXPECT_EQ(header.patientSex, "Unknown");
    EXPECT_EQ(header.samplingIntervalString, "1x10^-3 (s)");
    EXPECT_EQ(header.events.size(), 0);
    EXPECT_EQ(header.sequenceCount, 12);
    EXPECT_EQ(header.channelCount, 6);
}

// Test for NihonKohdenData initialization with file name
TEST(NihonKohdenDataTest, InitializationWithFileName)
{
    NihonKohdenData nkData("test-file.MWF");

    Header header = nkData.getHeader();
    EXPECT_EQ(trimNulls(header.preamble), "MFR Monitoring Waveform");
    EXPECT_EQ(header.byteOrder, ByteOrder::ENDIAN_LITTLE);
    EXPECT_EQ(trimNulls(header.modelInfo), "NIHON KOHDEN^CNS6000^0, 5, 0, 9");
    EXPECT_EQ(header.measurementTimeISO, "2019-06-19T13:20:00");
    EXPECT_EQ(trimNulls(header.patientID), "12345");
    EXPECT_EQ(trimNulls(header.patientName), "TRWRU");
    EXPECT_EQ(header.birthDateISO, "N/A");
    EXPECT_EQ(header.patientSex, "Unknown");
    EXPECT_EQ(header.samplingIntervalString, "1x10^-3 (s)");
    EXPECT_EQ(header.events.size(), 0);
    EXPECT_EQ(header.sequenceCount, 12);
    EXPECT_EQ(header.channelCount, 6);
}

// Test for anonymize method
TEST(NihonKohdenDataTest, Anonymize)
{
    ByteVector bv = FileManager::readBinaryFile("test-file.MWF");
    NihonKohdenData nkData(bv);
    nkData.anonymize();
    nkData.writeToBinary("test-file-anonymized.MWF");

    NihonKohdenData nkDataAnonymized("test-file-anonymized.MWF");
    Header header = nkDataAnonymized.getHeader();
    EXPECT_EQ(trimNulls(header.patientID), "");
    EXPECT_EQ(trimNulls(header.patientName), "");
}

// Test for setChannelSelection method
TEST(NihonKohdenDataTest, SetChannelSelection)
{
    ByteVector bv = FileManager::readBinaryFile("test-file.MWF");
    NihonKohdenData nkData(bv);

    EXPECT_NO_THROW(nkData.setChannelSelection(0, false));
    EXPECT_THROW(nkData.setChannelSelection(17, true), std::runtime_error);
}

// Test for setIntervalSelection method
TEST(NihonKohdenDataTest, SetIntervalSelection)
{
    ByteVector bv = FileManager::readBinaryFile("test-file.MWF");
    NihonKohdenData nkData(bv);

    EXPECT_NO_THROW(nkData.setIntervalSelection(0, 10));
    EXPECT_THROW(nkData.setIntervalSelection(10, 5), std::runtime_error);
    EXPECT_THROW(nkData.setIntervalSelection(-1, 10), std::runtime_error);
    EXPECT_THROW(nkData.setIntervalSelection(0, -1), std::runtime_error);
}

// Test for printHexData method
TEST(NihonKohdenDataTest, PrintHexData)
{
    ByteVector bv = FileManager::readBinaryFile("test-file.MWF");
    NihonKohdenData nkData(bv);

    std::stringstream buffer;
    std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());

    nkData.printHexData();
    std::cout.rdbuf(old);

    std::string expected = "\n| Tag   | Length      | Contents\n"
                           "|-------|-------------|----------->\n"
                           "|    40 |          20 | 4D 46 52 20 4D 6F 6E 69 74 6F 72 69 6E 67 20 57 61 76 65 66 6F 72 6D 00 00 00 00 00 00 00 00 00\n"
                           "|    01 |          01 | 01\n"
                           "|    03 |          0A | 41 4E 53 49 20 58 33 2E 34 00\n"
                           "|    17 |          20 | 4E 49 48 4F 4E 20 4B 4F 48 44 45 4E 5E 43 4E 53 36 30 30 30 5E 30 2C 20 35 2C 20 30 2C 20 39 00\n"
                           "|    08 |          01 | 14\n"
                           "|    85 |          0B | E3 07 06 13 0D 14 00 00 00 00 00\n"
                           "|    82 |          11 | 31 32 33 34 35 00 00 00 00 00 00 00 00 00 00 00 00\n"
                           "|    03 |          0A | 55 54 46 2D 31 36 4C 45 00 00\n"
                           "|    81 |          42 | 54 00 52 00 57 00 52 00 55 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00\n"
                           "|    03 |          0A | 41 4E 53 49 20 58 33 2E 34 00\n"
                           "|    83 |          07 | FF FF FF FF FF FF FF\n"
                           "|    84 |          01 | 00\n"
                           "|    0B |          03 | 01 FD 01\n"
                           "|    06 |          02 | 0C 00\n"
                           "|    05 |          01 | 06\n"
                           "|    12 |          02 | 00 80\n"
                           "|    3F |          18 | Tag   | Length      | Contents\n"
                           "|       |             |-------|-------------|----------->\n"
                           "|       |             |    09 |          02 | 02 00\n"
                           "|       |             |    0A |          01 | 00\n"
                           "|       |             |    04 |          04 | 98 3A 00 00\n"
                           "|       |             |    0B |          03 | 01 FD 04\n"
                           "|       |             |    0C |          04 | 00 FA 02 00\n"
                           "|    3F |          18 | Tag   | Length      | Contents\n"
                           "|       |             |-------|-------------|----------->\n"
                           "|       |             |    09 |          02 | 07 00\n"
                           "|       |             |    0A |          01 | 00\n"
                           "|       |             |    04 |          04 | 98 3A 00 00\n"
                           "|       |             |    0B |          03 | 01 FD 04\n"
                           "|       |             |    0C |          04 | 00 FA 02 00\n"
                           "|    3F |          18 | Tag   | Length      | Contents\n"
                           "|       |             |-------|-------------|----------->\n"
                           "|       |             |    09 |          02 | 0A C0\n"
                           "|       |             |    0A |          01 | 00\n"
                           "|       |             |    04 |          04 | 4C 1D 00 00\n"
                           "|       |             |    0B |          03 | 01 FD 08\n"
                           "|       |             |    0C |          04 | 01 FD 7D 00\n"
                           "|    3F |          18 | Tag   | Length      | Contents\n"
                           "|       |             |-------|-------------|----------->\n"
                           "|       |             |    09 |          02 | 12 C0\n"
                           "|       |             |    0A |          01 | 00\n"
                           "|       |             |    04 |          04 | 4C 1D 00 00\n"
                           "|       |             |    0B |          03 | 01 FD 08\n"
                           "|       |             |    0C |          04 | 01 FD 7D 00\n"
                           "|    3F |          18 | Tag   | Length      | Contents\n"
                           "|       |             |-------|-------------|----------->\n"
                           "|       |             |    09 |          02 | 13 C0\n"
                           "|       |             |    0A |          01 | 00\n"
                           "|       |             |    04 |          04 | 4C 1D 00 00\n"
                           "|       |             |    0B |          03 | 01 FD 08\n"
                           "|       |             |    0C |          04 | 01 FD 7D 00\n"
                           "|    3F |          12 | Tag   | Length      | Contents\n"
                           "|       |             |-------|-------------|----------->\n"
                           "|       |             |    09 |          02 | 40 10\n"
                           "|       |             |    0A |          01 | 04\n"
                           "|       |             |    04 |          04 | 98 3A 00 00\n"
                           "|       |             |    0B |          03 | 01 FD 04\n"
                           "|    1E |    18 B8 20 | ...\n"
                           "|    80 |          00 | End of file.\n";

    EXPECT_EQ(trimNulls(buffer.str()), trimNulls(expected));
}

// Test for printHeader method
TEST(NihonKohdenDataTest, PrintHeader)
{
    ByteVector bv = FileManager::readBinaryFile("test-file.MWF");
    NihonKohdenData nkData(bv);

    std::stringstream buffer;
    std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());

    nkData.printHeader();
    std::cout.rdbuf(old);

    std::string expected = "\nHeader: \n"
                           "Preamble: MFR Monitoring Waveform\n"
                           "Byte Order: Little Endian\n"
                           "Model Info: NIHON KOHDEN^CNS6000^0, 5, 0, 9\n"
                           "Measurement Time: 2019-06-19T13:20:00\n"
                           "Patient ID: 12345\n"
                           "Patient Name: TRWRU\n"
                           "Birth Date: N/A\n"
                           "Patient Sex: Unknown\n"
                           "Sampling Interval: 1x10^-3 (s)\n"
                           "NIBP Events: 0\n"
                           "Sequence Count: 12\n"
                           "Channel Count: 6\n"
                           "Channel: ECG II\n"
                           "   Block Length: 15000\n"
                           "   Sampling Resolution: 2x10^-6 (V)\n"
                           "Channel: ECG V5\n"
                           "   Block Length: 15000\n"
                           "   Sampling Resolution: 2x10^-6 (V)\n"
                           "Channel: ART\n"
                           "   Block Length: 7500\n"
                           "   Sampling Resolution: 125x10^-3 (mmHg)\n"
                           "Channel: PAP\n"
                           "   Block Length: 7500\n"
                           "   Sampling Resolution: 125x10^-3 (mmHg)\n"
                           "Channel: CVP\n"
                           "   Block Length: 7500\n"
                           "   Sampling Resolution: 125x10^-3 (mmHg)\n"
                           "Channel: Pacing Status\n"
                           "   Block Length: 15000\n"
                           "   Sampling Resolution: N/A\n\n";

    EXPECT_EQ(trimNulls(buffer.str()), trimNulls(expected));
}

// Test for writeToBinary method
TEST(NihonKohdenDataTest, WriteToBinary)
{
    ByteVector bv = FileManager::readBinaryFile("test-file.MWF");
    NihonKohdenData nkData(bv);
    std::string fileName = "output.mwf";

    nkData.writeToBinary(fileName);

    ByteVector writtenData = FileManager::readBinaryFile(fileName);
    ASSERT_EQ(writtenData.size(), bv.size()); // Ensure sizes match before comparing content
    EXPECT_TRUE(std::equal(writtenData.begin(), writtenData.end(), bv.begin()));
}

// Test for writeToCsv method
TEST(NihonKohdenDataTest, WriteToCsv)
{
    ByteVector bv = FileManager::readBinaryFile("test-file.MWF");
    NihonKohdenData nkData(bv);
    std::string fileName = "output.csv";

    nkData.writeToCsv(fileName);

    std::ifstream file(fileName);
    ASSERT_TRUE(file.is_open());

    std::string line;
    std::getline(file, line);
    EXPECT_EQ(line, "Time: (s), ECG II: 2x10^-6 (V), ECG V5: 2x10^-6 (V), ART: 125x10^-3 (mmHg), PAP: 125x10^-3 (mmHg), CVP: 125x10^-3 (mmHg), Pacing Status: N/A");

    file.close();
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
