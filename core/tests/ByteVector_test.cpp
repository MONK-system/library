#include <gtest/gtest.h>
#include "ByteVector.h"

// Test for stringToEncoding function
TEST(ByteVectorTest, StringToEncoding) {
    EXPECT_EQ(Encoding::ASCII, stringToEncoding("ASCII"));
    EXPECT_EQ(Encoding::UTF8, stringToEncoding("UTF-8"));
    EXPECT_EQ(Encoding::UTF16LE, stringToEncoding("UTF-16LE"));
    EXPECT_THROW(stringToEncoding("Unsupported"), std::runtime_error);
}

// Test for stringify method
TEST(ByteVectorTest, Stringify) {
    ByteVector bv = {0xAB, 0xCD, 0x45};
    EXPECT_EQ("AB CD 45", bv.stringify());
}

// Test for toString method
TEST(ByteVectorTest, ToString) {
    ByteVector bv = {0x48, 0x65, 0x6C, 0x6C, 0x6F};  // 'Hello' in ASCII
    EXPECT_EQ("Hello", bv.toString(Encoding::ASCII));
    EXPECT_THROW(bv.toString(Encoding::UTF16LE), std::runtime_error); // Handling binary to string conversion error for unsupported encoding in test case
}

// Test for toInt method
TEST(ByteVectorTest, ToInt) {
    ByteVector bv = {0x00, 0x00, 0x01, 0x02};
    EXPECT_EQ(258, bv.toInt<uint32_t>());
    EXPECT_THROW(bv.toInt<int8_t>(), std::runtime_error);  // Test for size larger than type
}

// Test for fromInt method
TEST(ByteVectorTest, FromInt) {
    auto bv = ByteVector::fromInt(258);
    ByteVector expected = {0x01, 0x02};
    EXPECT_EQ(expected, bv);

    bv = ByteVector::fromInt(0);
    expected = {0x00};
    EXPECT_EQ(expected, bv);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
