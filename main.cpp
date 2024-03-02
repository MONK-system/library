#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include "MFERDataCollection.h"
#include "FileManager.h"
#include "NihonKohdenData.h"

using namespace std;

int main(int argc, char *argv[])
{
    string inputFilename;
    string outputFilename = "output.csv"; // Default output filename

    // Argument parsing
    for (int i = 1; i < argc; i += 2)
    {
        if (string(argv[i]) == "-i" && i + 1 < argc)
        {
            inputFilename = argv[i + 1];
        }
        else if (string(argv[i]) == "-o" && i + 1 < argc)
        {
            outputFilename = argv[i + 1];
        }
        else
        {
            cerr << "Usage: " << argv[0] << " -i <InputFilename> [-o <OutputFilename>]" << endl;
            return 1;
        }
    }

    // Ensure the input filename is provided
    if (inputFilename.empty())
    {
        cerr << "Input filename is required." << endl;
        cerr << "Usage: " << argv[0] << " -i <InputFilename> [-o <OutputFilename>]" << endl;
        return 1;
    }

    try
    {
        NihonKohdenData nihonKohdenData(inputFilename);

        nihonKohdenData.printData();
        nihonKohdenData.printDataFields();
        nihonKohdenData.writeWaveformToCsv(outputFilename);
    }
    catch (const exception &e)
    {
        cerr << "Error reading input file: " << e.what() << endl;
        return 1;
    }

    cout << endl;

    return 0;
}