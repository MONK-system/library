#include <iostream>
#include <sstream>
#include "MFERDataCollection.h"
#include "MFERData.h"

using namespace std;

MFERDataCollection::MFERDataCollection(ByteVector dataVector)
{
    mferDataVector = parseMFERDataCollection(dataVector);
}

string MFERDataCollection::toString(int maxByteLength) const
{
    MFERData::maxByteLength = maxByteLength;
    return collectionToString(&mferDataVector, "");
}

vector<unique_ptr<MFERData>> parseMFERDataCollection(ByteVector dataVector)
{
    DataStack dataBlock(dataVector);
    vector<unique_ptr<MFERData>> collection;

    while (dataBlock.size() > 0)
    {
        try
        {
            unique_ptr<MFERData> data = parseMFERData(&dataBlock);

            // Add to collection
            collection.push_back(move(data));

            // Exit if at end of file (tag 80)
            if (dataBlock.size() <= 0)
            {
                break;
            }
        }
        catch (const runtime_error &e)
        {
            throw e;
        }
    }
    return collection;
}

string collectionToString(const vector<unique_ptr<MFERData>> *collection, string left)
{
    ostringstream stream;
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
        stream.seekp(-1, ios_base::end);
    }

    return stream.str();
}