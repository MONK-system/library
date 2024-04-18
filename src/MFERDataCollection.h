#ifndef MFERDATACOLLECTION_H
#define MFERDATACOLLECTION_H

#include "MFERDataCollection.h"
#include "MFERData.h"
#include "DataStack.h"
#include <vector>

class MFERData;

class MFERDataCollection
{
public:
    MFERDataCollection() = default;
    MFERDataCollection(ByteVector dataVector);

    inline const std::vector<std::unique_ptr<MFERData>> &getMFERDataVector() const
    {
        return mferDataVector;
    };
    inline size_t size() const { return mferDataVector.size(); };
    std::string toHexString(uint64_t maxByteLength = 100) const;
    ByteVector toByteVector() const;

private:
    std::vector<std::unique_ptr<MFERData>> mferDataVector;
};

std::string collectionToHexString(const std::vector<std::unique_ptr<MFERData>> *collection, std::string left);
std::vector<std::unique_ptr<MFERData>> parseMFERDataCollection(ByteVector dataVector);

#endif
