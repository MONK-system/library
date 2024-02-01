#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>

struct MWFData
{
    std::vector<unsigned char> tag;
    std::vector<unsigned char> length;
    std::vector<unsigned char> contents;
};

int hexVectorToInt(const std::vector<unsigned char> &hexVector)
{
    int result = 0;

    for (unsigned char byte : hexVector)
    {
        result = (result << 8) | byte;
    }

    return result;
}

int main(int argc, char* argv[]) {
    if (argc != 3 || std::string(argv[1]) != "-i") {
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

    std::vector<MWFData> dataCollection;

    while (file)
    {
        MWFData data;

        // Read Tag (first byte)
        unsigned char firstTagByte;
        if (!file.read(reinterpret_cast<char *>(&firstTagByte), sizeof(firstTagByte)))
        {
            break; // Exit loop if read fails (likely end of file)
        }

        if (firstTagByte == 0x80)
        {
            data.tag.push_back(firstTagByte);
            data.length.push_back(0x00);
            dataCollection.push_back(data);
            break; // Stop reading if tag is 80
        }

        if (firstTagByte == 0x00)
        {
            std::cerr << "Encountered invalid tag (00), stopping read." << std::endl;
            break; // Stop reading if tag is 00
        }

        data.tag.push_back(firstTagByte);

        // Special case for tag 3F: read next byte as part of the tag
        if (firstTagByte == 0x3F)
        {
            unsigned char secondTagByte;
            if (!file.read(reinterpret_cast<char *>(&secondTagByte), sizeof(secondTagByte)))
            {
                std::cerr << "Error reading second byte of tag 3F." << std::endl;
                return 1;
            }
            data.tag.push_back(secondTagByte);
        }

        // Read Length
        if (firstTagByte != 0x1E)
        {
            unsigned char lengthByte;
            if (!file.read(reinterpret_cast<char *>(&lengthByte), sizeof(lengthByte)))
            {
                std::cerr << "Error reading length from file." << std::endl;
                return 1;
            }
            data.length.push_back(lengthByte);
        }
        else
        {
            // Get word length
            unsigned char wordLength;
            if (!file.read(reinterpret_cast<char *>(&wordLength), sizeof(wordLength)))
            {
                std::cerr << "Error reading length from file." << std::endl;
                return 1;
            }

            // Read length
            int intLength = static_cast<int>(wordLength);
            unsigned char bytes[intLength - 128];
            if (!file.read(reinterpret_cast<char *>(bytes), sizeof(bytes)))
            {
                std::cerr << "Error reading bytes from the file!" << std::endl;
                return 1;
            }

            data.length.assign(bytes, bytes + sizeof(bytes));
        }

        // Read Contents
        data.contents.resize(hexVectorToInt(data.length));
        if (!file.read(reinterpret_cast<char *>(data.contents.data()), data.contents.size()))
        {
            std::cerr << "Error reading contents from file." << std::endl;
            return 1;
        }

        // Add to collection
        dataCollection.push_back(data);
    }

    file.close();

    // Print Header
    std::cout << "| Tag    | Length      | Contents" << std::endl;
    std::cout << "|--------|-------------|------------>" << std::endl;

    // Output the data for verification
    for (const auto &data : dataCollection)
    {
        // Print Tag
        std::cout << "| ";
        if (data.tag.size() <= 2)
        {
            std::cout << " ";
        }
        for (auto val : data.tag)
        {
            std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)val << " ";
        }
        std::cout << std::string(6 - data.tag.size() * 3, ' '); // Adjust spacing

        // Print Length
        std::cout << "| ";
        for (auto val : data.length)
        {
            std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)val << " ";
        }
        std::cout << std::string(12 - data.length.size() * 3, ' '); // Adjust spacing

        // Print Contents
        std::cout << "| ";
        if (data.contents.size() < 100) // Don't print data with too much content
        {
            for (auto val : data.contents)
            {
                std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)val << " ";
            }
        }
        else
        {
            std::cout << "[Too long to display]";
        }
        std::cout << " |" << std::endl;
    }

    return 0;
}
