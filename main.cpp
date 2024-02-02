#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include "MFERData.h"

int main(int argc, char *argv[])
{
    if (argc != 3 || std::string(argv[1]) != "-i")
    {
        std::cerr << "Usage: " << argv[0] << " -i <Filename>" << std::endl;
        return 1;
    }

    std::string filename = argv[2];
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return 1;
    }

    DataBlock dataBlock(&file);

    file.close();

    std::vector<MFERData> dataCollection;

    // Print header
    std::cout << std::endl
              << MFERData::headerString() << std::endl;

    while (dataBlock.size() > 0)
    {
        try
        {
            MFERData data = MFERData(&dataBlock);

            // Add to collection
            dataCollection.push_back(data);

            // Output data for verification
            std::cout << data.toString(100) << std::endl;

            // Exit if at end of file (tag 80)
            if (data.getTag() == 0x80)
            {
                break;
            }
        }
        catch (const std::runtime_error &e)
        {
            std::cerr << "Failed to create object: " << e.what() << std::endl;
            break;
        }
    }

    return 0;
}
