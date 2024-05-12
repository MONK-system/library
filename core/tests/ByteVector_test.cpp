#include <gtest/gtest.h>
#include "ByteVector.h"

// Test for stringify method
TEST(ByteVectorTest, Stringify)
{
    ByteVector bv = {0xAB, 0xCD, 0x45};
    EXPECT_EQ("AB CD 45", bv.stringify());
}

// Test for toString method
TEST(ByteVectorTest, ToString)
{
    ByteVector bv = {0x48, 0x65, 0x6C, 0x6C, 0x6F}; // 'Hello' in ASCII
    EXPECT_EQ("Hello", bv.toString(Encoding::ASCII));
    EXPECT_THROW(bv.toString(Encoding::UTF16LE), std::runtime_error);  // Handling binary to string conversion error for unsupported encoding
    bv = {0x48, 0x00, 0x65, 0x00, 0x6C, 0x00, 0x6C, 0x00, 0x6F, 0x00}; // 'Hello' in UTF-16LE
    EXPECT_EQ("Hello", bv.toString(Encoding::UTF16LE));
}

// Test for toInt method
TEST(ByteVectorTest, ToInt)
{
    ByteVector bv = {0x00, 0x00, 0x01, 0x02};
    EXPECT_EQ(258, bv.toInt<uint32_t>());
    EXPECT_THROW(bv.toInt<int8_t>(), std::runtime_error); // Test for size larger than type
}

// Test for fromInt method
TEST(ByteVectorTest, FromInt)
{
    auto bv = ByteVector::fromInt(258);
    ByteVector expected = {0x01, 0x02};
    EXPECT_EQ(expected, bv);

    bv = ByteVector::fromInt(0);
    expected = {0x00};
    EXPECT_EQ(expected, bv);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
