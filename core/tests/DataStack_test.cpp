#include <gtest/gtest.h>
#include "DataStack.h"

// Test for DataStack initialization
TEST(DataStackTest, Initialization)
{
    ByteVector bv = {0x01, 0x02, 0x03};
    DataStack ds(bv);
    EXPECT_EQ(ds.size(), 3);
}

// Test for pop_front method
TEST(DataStackTest, PopFront)
{
    ByteVector bv = {0x01, 0x02, 0x03};
    DataStack ds(bv);

    ByteVector popped = ds.pop_front(2);
    ByteVector expected = {0x01, 0x02};

    EXPECT_EQ(popped, expected);
    EXPECT_EQ(ds.size(), 1);
}

// Test for pop_byte method
TEST(DataStackTest, PopByte)
{
    ByteVector bv = {0x01, 0x02};
    DataStack ds(bv);

    uint8_t byte = ds.pop_byte();
    EXPECT_EQ(byte, 0x01);
    EXPECT_EQ(ds.size(), 1);
}

// Test for read_byte method
TEST(DataStackTest, ReadByte)
{
    ByteVector bv = {0x01, 0x02};
    DataStack ds(bv);

    uint8_t byte = ds.read_byte();
    EXPECT_EQ(byte, 0x01);
    EXPECT_EQ(ds.size(), 2);
}

// Test for size method
TEST(DataStackTest, Size)
{
    ByteVector bv = {0x01, 0x02, 0x03, 0x04};
    DataStack ds(bv);
    EXPECT_EQ(ds.size(), 4);
}

// Test for pop_value method
TEST(DataStackTest, PopValue)
{
    ByteVector bv = {0x00, 0x00, 0x01, 0x02};
    DataStack ds(bv);

    uint32_t value = ds.pop_value<uint32_t>();
    EXPECT_EQ(value, 258);
    EXPECT_EQ(ds.size(), 0);
}

// Test for pop_values method
TEST(DataStackTest, PopValues)
{
    ByteVector bv = {0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0x03, 0x04};
    DataStack ds(bv);

    auto values = ds.pop_values<uint32_t>(2);
    std::vector<uint32_t> expected = {258, 772};

    EXPECT_EQ(values, expected);
    EXPECT_EQ(ds.size(), 0);
}

// Test for pop_double method
TEST(DataStackTest, PopDouble)
{
    ByteVector bv = {0x40, 0x09, 0x21, 0xFB, 0x54, 0x44, 0x2D, 0x18}; // 3.141592653589793 in double (big endian)
    DataStack ds(bv);

    double value = ds.pop_double<double>();
    EXPECT_DOUBLE_EQ(value, 3.141592653589793);
    EXPECT_EQ(ds.size(), 0);
}

// Test for pop_doubles method
TEST(DataStackTest, PopDoubles)
{
    ByteVector bv = {
        0x40, 0x09, 0x21, 0xFB, 0x54, 0x44, 0x2D, 0x18,  // 3.141592653589793
        0x40, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   // 4.0
    };
    DataStack ds(bv);

    auto values = ds.pop_doubles<double>(2);
    std::vector<double> expected = {3.141592653589793, 4.0};

    EXPECT_EQ(values, expected);
    EXPECT_EQ(ds.size(), 0);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
