#include <gtest/gtest.h>
#include "MFERData.h"
#include "ByteVector.h"
#include "DataStack.h"

// Test for MFERData initialization
TEST(MFERDataTest, Initialization)
{
    ByteVector bv = {0x05, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E};
    DataStack ds(bv);
    MFERData mferData(&ds);

    EXPECT_EQ(mferData.getLength(), 5);
    EXPECT_EQ(mferData.getContents(), ByteVector({0x0A, 0x0B, 0x0C, 0x0D, 0x0E}));
}

// Test for parsing PRE tag
TEST(MFERDataTest, ParsePRE)
{
    ByteVector bv = {0x40, 0x03, 0x0A, 0x0B, 0x0C};
    DataStack ds(bv);
    auto preData = parseMFERData(&ds);

    EXPECT_EQ(preData->getTag(), 0x40);
    EXPECT_EQ(preData->getContents(), ByteVector({0x0A, 0x0B, 0x0C}));
}

// Test for BLE tag and getByteOrder method
TEST(MFERDataTest, BLEGetByteOrder)
{
    ByteVector bv = {0x01, 0x01, static_cast<uint8_t>(ByteOrder::ENDIAN_BIG)};
    DataStack ds(bv);
    auto bleData = parseMFERData(&ds);

    EXPECT_EQ(bleData->getTag(), 0x01);
    EXPECT_EQ(dynamic_cast<BLE *>(bleData.get())->getByteOrder(), ByteOrder::ENDIAN_BIG);
}

// Test for TXC tag and getEncoding method
TEST(MFERDataTest, TXCGetEncoding)
{
    ByteVector bv = {0x03, 0x05, 'U', 'T', 'F', '-', '8'};
    DataStack ds(bv);
    auto txcData = parseMFERData(&ds);

    EXPECT_EQ(txcData->getTag(), 0x03);
    EXPECT_EQ(dynamic_cast<TXC *>(txcData.get())->getEncoding(), Encoding::UTF8);
}

// Test for PID anonymize method
TEST(MFERDataTest, PIDAnonymize)
{
    ByteVector bv = {0x82, 0x03, 0x0A, 0x0B, 0x0C};
    DataStack ds(bv);
    auto pidData = parseMFERData(&ds);

    pidData->anonymize();
    EXPECT_EQ(pidData->getContents(), ByteVector(3, 0x00));
}

// Test for SEX getPatientSex method
TEST(MFERDataTest, SEXGetPatientSex)
{
    ByteVector bv = {0x84, 0x01, 0x01};
    DataStack ds(bv);
    auto sexData = parseMFERData(&ds);

    EXPECT_EQ(sexData->getTag(), 0x84);
    EXPECT_EQ(dynamic_cast<SEX *>(sexData.get())->getPatientSex(), "Male");
}

// Test for AGE anonymize method
TEST(MFERDataTest, AGEAnonymize)
{
    ByteVector bv = {0x83, 0x03, 0x1A, 0x1B, 0x1C};
    DataStack ds(bv);
    auto ageData = parseMFERData(&ds);

    ageData->anonymize();
    EXPECT_EQ(ageData->getContents(), ByteVector(3, 0xFF));
}

// Test for WFM getWaveformType method
TEST(MFERDataTest, WFMGetWaveformType)
{
    ByteVector bv = {0x08, 0x01, 0x02};
    DataStack ds(bv);
    auto wfmData = parseMFERData(&ds);

    EXPECT_EQ(wfmData->getTag(), 0x08);
    EXPECT_EQ(dynamic_cast<WFM *>(wfmData.get())->getWaveformType(), 0x02);
}

// Test for TIM getMeasurementTime method
TEST(MFERDataTest, TIMGetMeasurementTime)
{
    ByteVector bv = {0x85, 0x0B, 0xD6, 0x07, 0x0B, 0x05, 0x06, 0x2C, 0x00, 0x00, 0x00, 0x00, 0x00};
    DataStack ds(bv);
    auto timData = parseMFERData(&ds);

    EXPECT_EQ(timData->getTag(), 0x85);
    EXPECT_EQ(dynamic_cast<TIM *>(timData.get())->getMeasurementTime(ByteOrder::ENDIAN_LITTLE), "2006-11-05T06:44:00");
}

// Test for IVL getSamplingInterval method
TEST(MFERDataTest, IVLGetSamplingInterval)
{
    ByteVector bv = {0x0B, 0x03, 0x01, 0xFD, 0x01};
    DataStack ds(bv);
    auto ivlData = parseMFERData(&ds);

    EXPECT_EQ(ivlData->getTag(), 0x0B);
    EXPECT_FLOAT_EQ(dynamic_cast<IVL *>(ivlData.get())->getSamplingInterval(), 0.001);
}

// Test required for NIBPEvent, specification unavailable

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
