#ifndef HEXVECTOR_H
#define HEXVECTOR_H

#include <vector>
#include <type_traits>
#include <sstream>
#include <iomanip>
#include <locale>
#include <codecvt>
#include <string>
#include <algorithm>
#include <iostream>
#include <cstring>

using namespace std;

enum class ByteOrder
{
    ENDIAN_BIG,
    ENDIAN_LITTLE
};

// Creates a string "AB CD 45 " from a list of bytes, for example
inline string stringifyBytes(vector<unsigned char> bytes)
{
    ostringstream stream;
    for (auto val : bytes)
    {
        stream << hex << uppercase << setw(2) << setfill('0') << (int)val << " ";
    }
    string string = stream.str();
    string.pop_back();
    return string;
}

template <typename T>
T hexVectorToInt(const vector<unsigned char> hexVector, ByteOrder byteOrder = ByteOrder::ENDIAN_BIG)
{
    static_assert(is_integral<T>::value || is_floating_point<T>::value, "Return type must be an integral or floating-point type.");
    if (hexVector.size() > sizeof(T))
    {
        throw runtime_error("Vector size is larger than the size of the return type.");
    }

    T value;
    // If the byte order needs to be reversed for this system, reverse the vector before copying.
    if (byteOrder == ByteOrder::ENDIAN_BIG)
    {
        // Systems are typically little-endian; reverse if original data is big-endian.
        vector<unsigned char> reversedHexVector(hexVector.rbegin(), hexVector.rend());
        memcpy(&value, reversedHexVector.data(), sizeof(T));
    }
    else
    {
        // If the system's byte order matches the data's byte order, copy directly.
        memcpy(&value, hexVector.data(), sizeof(T));
    }

    return value;
}

template int hexVectorToInt<int>(const vector<unsigned char>, ByteOrder);
template long long hexVectorToInt<long long>(const vector<unsigned char>, ByteOrder);
template double hexVectorToInt<double>(const vector<unsigned char>, ByteOrder);

template <typename T>
vector<unsigned char> intToHexVector(T value)
{
    static_assert(is_integral<T>::value, "Input type must be an integral type.");

    vector<unsigned char> hexVector;
    bool leadingZero = true;

    for (int i = (sizeof(T) - 1) * 8; i >= 0; i -= 8)
    {
        unsigned char byte = (value >> i) & 0xFF;
        if (byte != 0 || !leadingZero)
        {
            hexVector.push_back(byte);
            leadingZero = false;
        }
    }

    // In case the value is 0, ensure the vector is not empty
    if (hexVector.empty())
    {
        hexVector.push_back(0);
    }

    return hexVector;
}

// Enum to represent different character encodings
enum class Encoding
{
    ASCII,
    UTF8,
    UTF16LE
};

// String to encoding
inline Encoding stringToEncoding(const string &encoding)
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

// Struct to hold a wstring and its encoding
struct EncodedString
{
    wstring str;
    Encoding encoding;
};

inline string convertToString(const vector<unsigned char> bytes)
{
    return string(bytes.begin(), bytes.end());
}

// Of note, wstring_convert is deprecated in C++17, function should later be updated to use https://icu.unicode.org libraries
inline wstring convertToWstring(const vector<unsigned char> bytes, Encoding encoding)
{
    string str(bytes.begin(), bytes.end());
    if (encoding == Encoding::ASCII || encoding == Encoding::UTF8)
    {
        wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
        return converter.from_bytes(str);
    }
    else if (encoding == Encoding::UTF16LE)
    {
        wstring_convert<codecvt_utf8_utf16<char16_t, 0x10ffff, codecvt_mode::little_endian>, char16_t> converter;
        u16string u16str = converter.from_bytes(str);
        return wstring(u16str.begin(), u16str.end());
    }
    else
    {
        throw runtime_error("Unsupported encoding.");
    }
};

inline EncodedString convertToEncodedString(const vector<unsigned char> bytes, Encoding encoding)
{
    EncodedString encodedString = {convertToWstring(bytes, encoding), encoding};
    return encodedString;
};

#endif