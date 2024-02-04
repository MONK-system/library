#include <sstream>
#include "MFERDataCollection.h"
#include "MFERData.h"

MFERDataCollection::MFERDataCollection(std::vector<unsigned char> dataVector)
{
    dataCollection = parseMFERDataCollection(dataVector);
}

std::string MFERDataCollection::toString(int maxByteLength) const
{
    return contentsString(dataCollection, maxByteLength, "");
}

std::vector<MFERData> parseMFERDataCollection(std::vector<unsigned char> dataVector)
{
    DataStack dataBlock(dataVector);
    std::vector<MFERData> collection;

    while (dataBlock.size() > 0)
    {
        try
        {
            MFERData data = MFERData(&dataBlock);

            // Add to collection
            collection.push_back(data);

            // Exit if at end of file (tag 80)
            if (data.getTag() == 0x80)
            {
                break;
            }
        }
        catch (const std::runtime_error &e)
        {
            throw e;
        }
    }
    return collection;
}

std::string contentsString(const std::vector<MFERData> collection, int maxByteLength, std::string left)
{
    std::ostringstream stream;
    stream << MFERData::headerString() << "\n"
           << left << MFERData::sectionString();
    for (MFERData data : collection)
    {
        stream << "\n" << data.toString(maxByteLength, left);
    }
    stream.seekp(-1, std::ios_base::end);
    return stream.str();
}