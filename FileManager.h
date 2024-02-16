#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

class FileManager {
public:
    static std::vector<unsigned char> readBinaryFile(const std::string& fileName);
};

#endif // FILEMANAGER_H