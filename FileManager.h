#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "ByteVector.h"
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

using namespace std;

class FileManager
{
public:
    FileManager(const string &outputFileName);
    ~FileManager(); // Declare the destructor

    void writeLine(const string &line);
    void writeLines(const vector<string> &lines);
    void closeFile();

    static ByteVector readBinaryFile(const string &fileName);

private:
    string fileName;
    ofstream outputFile;
};

#endif // FILEMANAGER_H