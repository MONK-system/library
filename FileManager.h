#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

class FileManager
{
public:
    FileManager(const std::string &outputFileName);
    void setOutputFormat(const std::string &format); // If future file formats are needed
    void writeLines(const std::vector<std::string> &lines);
    void closeFile();

    static std::vector<unsigned char> readBinaryFile(const std::string &fileName);

private:
    std::string fileName;
    std::string outputFormat;
    std::ofstream outputFile;
};

#endif // FILEMANAGER_H