#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "ByteVector.h"
#include <vector>
#include <string>
#include <fstream>

class FileManager
{
public:
    FileManager(const std::string &outputFileName);
    ~FileManager(); // Declare the destructor

    void writeLine(const std::string &line);
    void writeLines(const std::vector<std::string> &lines);
    void closeFile();

    static ByteVector readBinaryFile(const std::string &fileName);

private:
    std::string fileName;
    std::ofstream outputFile;
};

#endif // FILEMANAGER_H
