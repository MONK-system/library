#include <gtest/gtest.h>
#include "MFERDataCollection.h"
#include "MFERData.h"
#include "ByteVector.h"

// Test for MFERDataCollection initialization
TEST(MFERDataCollectionTest, Initialization)
{
    ByteVector bv = {0x40, 0x03, 0x0A, 0x0B, 0x0C, 0x80, 0x00};
    MFERDataCollection collection(bv);

    EXPECT_EQ(collection.size(), 2); // 2 because we have a PRE and END tag
}

// Test for toHexString method
TEST(MFERDataCollectionTest, ToHexString)
{
    ByteVector bv = {0x40, 0x03, 0x0A, 0x0B, 0x0C, 0x80, 0x00};
    MFERDataCollection collection(bv);

    std::string hexString = collection.toHexString();
    std::string expected = "| Tag   | Length      | Contents\n"
                           "|-------|-------------|----------->\n"
                           "|    40 |          03 | 0A 0B 0C\n"
                           "|    80 |          00 | End of file.";

    EXPECT_EQ(hexString, expected);
}

// Test for toByteVector method
TEST(MFERDataCollectionTest, ToByteVector)
{
    ByteVector bv = {0x40, 0x03, 0x0A, 0x0B, 0x0C, 0x80, 0x00};
    MFERDataCollection collection(bv);

    ByteVector byteVector = collection.toByteVector();
    EXPECT_EQ(byteVector, bv);
}

// Test for parseMFERDataCollection function
TEST(MFERDataCollectionTest, ParseMFERDataCollection)
{
    ByteVector bv = {0x40, 0x03, 0x0A, 0x0B, 0x0C, 0x80, 0x00};
    auto collection = parseMFERDataCollection(bv);

    EXPECT_EQ(collection.size(), 2);
    EXPECT_EQ(collection[0]->getTag(), 0x40);
    EXPECT_EQ(collection[0]->getContents(), ByteVector({0x0A, 0x0B, 0x0C}));
    EXPECT_EQ(collection[1]->getTag(), 0x80);
}

// Test for collectionToHexString function
TEST(MFERDataCollectionTest, CollectionToHexString)
{
    ByteVector bv = {0x40, 0x03, 0x0A, 0x0B, 0x0C, 0x80, 0x00};
    auto collection = parseMFERDataCollection(bv);

    std::string hexString = collectionToHexString(&collection, "");
    std::string expected = "| Tag   | Length      | Contents\n"
                           "|-------|-------------|----------->\n"
                           "|    40 |          03 | 0A 0B 0C\n"
                           "|    80 |          00 | End of file.";

    EXPECT_EQ(hexString, expected);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
