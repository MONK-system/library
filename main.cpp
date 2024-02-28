#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include "MFERDataCollection.h"
#include "FileManager.h"
#include "NihonKohdenData.h"

int main(int argc, char *argv[])
{
    if (argc != 3 || std::string(argv[1]) != "-i")
    {
        std::cerr << "Usage: " << argv[0] << " -i <Filename>" << std::endl;
        return 1;
    }

    std::string filename = argv[2];

    NihonKohdenData nihonKohdenData(filename);

    nihonKohdenData.printData();
    nihonKohdenData.printDataFields();
    nihonKohdenData.writeWaveformToFile("output", 2);

    return 0;
}