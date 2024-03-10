#include "ByteVector.h"
#include <codecvt>
#include <cstdint>
#include <vector>
#include <sstream>

using namespace std;

string ByteVector::stringify() const
{
    ostringstream stream;
    for (auto val : *this)
    {
        stream << hex << uppercase << setw(2) << setfill('0') << (int)val << " ";
    }
    string string = stream.str();
    string.pop_back();
    return string;
}

// Of note, wstring_convert is deprecated in C++17, function should later be updated to use https://icu.unicode.org libraries
string ByteVector::toString(Encoding encoding) const
{
    string str(this->begin(), this->end());
    if (encoding == Encoding::ASCII || encoding == Encoding::UTF8)
    {
        return str;
    }
    else if (encoding == Encoding::UTF16LE)
    {
        wstring_convert<codecvt_utf8_utf16<char16_t, 0x10ffff, codecvt_mode::little_endian>, char16_t> converter;
        u16string u16str = converter.from_bytes(str);
        return converter.to_bytes(u16str);
    }
    else
    {
        throw runtime_error("Unsupported encoding.");
    }
}

Encoding stringToEncoding(const string &encoding)
{
    if (encoding.find("ASCII") != string::npos || encoding.find("ANSI X3.4") != string::npos)
    {
        return Encoding::ASCII;
    }
    else if (encoding.find("UTF-8") != string::npos)
    {
        return Encoding::UTF8;
    }
    else if (encoding.find("UTF-16LE") != string::npos)
    {
        return Encoding::UTF16LE;
    }
    else
    {
        throw runtime_error("Unsupported encoding.");
    }
}