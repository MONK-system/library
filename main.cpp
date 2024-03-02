#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include "MFERDataCollection.h"
#include "FileManager.h"
#include "NihonKohdenData.h"

int main(int argc, char *argv[])
{
    std::string inputFilename;
    std::string outputFilename = "output.csv"; // Default output filename

    // Argument parsing
    for (int i = 1; i < argc; i += 2)
    {
        if (std::string(argv[i]) == "-i" && i + 1 < argc)
        {
            inputFilename = argv[i + 1];
        }
        else if (std::string(argv[i]) == "-o" && i + 1 < argc)
        {
            outputFilename = argv[i + 1];
        }
        else
        {
            std::cerr << "Usage: " << argv[0] << " -i <InputFilename> [-o <OutputFilename>]" << std::endl;
            return 1;
        }
    }

    // Ensure the input filename is provided
    if (inputFilename.empty())
    {
        std::cerr << "Input filename is required." << std::endl;
        std::cerr << "Usage: " << argv[0] << " -i <InputFilename> [-o <OutputFilename>]" << std::endl;
        return 1;
    }

    try
    {
        NihonKohdenData nihonKohdenData(inputFilename);

        nihonKohdenData.printData();
        nihonKohdenData.printDataFields();
        nihonKohdenData.writeWaveformToCsv(outputFilename);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error reading input file: " << e.what() << std::endl;
        return 1;
    }

    std::cout << std::endl;

    return 0;
}