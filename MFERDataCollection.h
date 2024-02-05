#ifndef MFERDATACOLLECTION_H
#define MFERDATACOLLECTION_H

#include <vector>
#include "MFERDataCollection.h"
#include "MFERData.h"
#include "DataStack.h"

class MFERData;

class MFERDataCollection
{
public:
    MFERDataCollection(std::vector<unsigned char> dataVector);

    inline size_t size() const { return dataCollection.size(); }
    std::string toString(int maxByteLength = 100) const;

private:
    std::vector<std::unique_ptr<MFERData>> dataCollection;
};

std::string collectionToString(const std::vector<std::unique_ptr<MFERData>>* collection, std::string left);
std::vector<std::unique_ptr<MFERData>> parseMFERDataCollection(std::vector<unsigned char> dataVector);

#endif