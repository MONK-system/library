#include <gtest/gtest.h>
#include "FileManager.h"
#include <fstream>
#include <vector>
#include <string>

// Helper function to read a text file into a vector of strings
std::vector<std::string> readTextFile(const std::string &fileName)
{
    std::ifstream file(fileName);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line))
    {
        lines.push_back(line);
    }
    return lines;
}

// Test for writing a single line to a CSV file
TEST(FileManagerTest, WriteLineCSV)
{
    FileManager fm("test.csv");
    fm.writeLine("Header1,Header2,Header3");
    fm.writeLine("Value1,Value2,Value3");
    fm.closeFile();

    auto lines = readTextFile("test.csv");
    ASSERT_EQ(lines.size(), 2);
    EXPECT_EQ(lines[0], "Header1,Header2,Header3");
    EXPECT_EQ(lines[1], "Value1,Value2,Value3");
}

// Test for writing multiple lines to a CSV file
TEST(FileManagerTest, WriteLinesCSV)
{
    FileManager fm("test_multiple.csv");
    std::vector<std::string> lines = {
        "Header1,Header2,Header3",
        "Value1,Value2,Value3",
        "Value4,Value5,Value6"
    };
    fm.writeLines(lines);
    fm.closeFile();

    auto readLines = readTextFile("test_multiple.csv");
    ASSERT_EQ(readLines.size(), 3);
    EXPECT_EQ(readLines[0], lines[0]);
    EXPECT_EQ(readLines[1], lines[1]);
    EXPECT_EQ(readLines[2], lines[2]);
}

// Test for writing and reading a binary MWF file
TEST(FileManagerTest, WriteAndReadBinaryMWF)
{
    ByteVector data = {0x01, 0x02, 0x03, 0x04, 0x05};
    FileManager::writeBinaryFile("test.mwf", data);

    ByteVector readData = FileManager::readBinaryFile("test.mwf");
    EXPECT_EQ(readData, data);
}

// Test for handling file open error
TEST(FileManagerTest, FileOpenError)
{
    std::string invalidFileName = "/invalid/path/test.csv";
    try
    {
        FileManager fm(invalidFileName);
    }
    catch (const std::runtime_error &e)
    {
        EXPECT_STREQ(e.what(), "Error opening output file: /invalid/path/test.csv");
    }
}

// Test for handling binary file read error
TEST(FileManagerTest, BinaryFileReadError)
{
    std::string nonExistentFile = "non_existent.mwf";
    try
    {
        ByteVector data = FileManager::readBinaryFile(nonExistentFile);
    }
    catch (const std::runtime_error &e)
    {
        EXPECT_STREQ(e.what(), "Error opening file.");
    }
}

// Test for handling binary file write error
TEST(FileManagerTest, BinaryFileWriteError)
{
    std::string invalidFileName = "/invalid/path/test.mwf";
    ByteVector data = {0x01, 0x02};
    try
    {
        FileManager::writeBinaryFile(invalidFileName, data);
    }
    catch (const std::runtime_error &e)
    {
        EXPECT_STREQ(e.what(), "Error opening file.");
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
