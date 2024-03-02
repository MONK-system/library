#ifndef MFERDATACOLLECTION_H
#define MFERDATACOLLECTION_H

#include <vector>
#include "MFERDataCollection.h"
#include "MFERData.h"
#include "DataStack.h"

using namespace std;

class MFERData;

class MFERDataCollection
{
public:
    MFERDataCollection() = default;
    MFERDataCollection(vector<unsigned char> dataVector);

    inline const vector<unique_ptr<MFERData>> &getMFERDataVector() const
    {
        return mferDataVector;
    };
    inline size_t size() const { return mferDataVector.size(); };
    string toString(int maxByteLength = 100) const;

private:
    vector<unique_ptr<MFERData>> mferDataVector;
};

string collectionToString(const vector<unique_ptr<MFERData>> *collection, string left);
vector<unique_ptr<MFERData>> parseMFERDataCollection(vector<unsigned char> dataVector);

#endif