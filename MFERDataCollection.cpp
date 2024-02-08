#include <iostream>
#include <sstream>
#include "MFERDataCollection.h"
#include "MFERData.h"

MFERDataCollection::MFERDataCollection(std::vector<unsigned char> dataVector)
{
    dataCollection = parseMFERDataCollection(dataVector);
}

std::string MFERDataCollection::toString(int maxByteLength) const
{
    MFERData::maxByteLength = maxByteLength;
    return collectionToString(&dataCollection, "");
}

std::vector<std::unique_ptr<MFERData>> parseMFERDataCollection(std::vector<unsigned char> dataVector)
{
    DataStack dataBlock(dataVector);
    std::vector<std::unique_ptr<MFERData>> collection;

    while (dataBlock.size() > 0)
    {
        try
        {
            std::unique_ptr<MFERData> data = parseMFERData(&dataBlock);

            // Add to collection
            collection.push_back(std::move(data));

            // Exit if at end of file (tag 80)
            if (dataBlock.size() <= 0)
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

std::string collectionToString(const std::vector<std::unique_ptr<MFERData>> *collection, std::string left)
{
    std::ostringstream stream;
    stream << MFERData::headerString() << "\n"
           << left << MFERData::sectionString();

    if (collection != nullptr)
    {
        for (const auto &data : *collection)
        {
            stream << "\n"
                   << data->toString(left);
        }
    }

    if (stream.tellp() > 0)
    {
        stream.seekp(-1, std::ios_base::end);
    }

    return stream.str();
}