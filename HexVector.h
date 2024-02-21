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

enum class ByteOrder : bool
{
    BIG_ENDIAN,
    LITTLE_ENDIAN
};

template <typename T>
T hexVectorToInt(const std::vector<unsigned char> hexVector, ByteOrder byteOrder = ByteOrder::BIG_ENDIAN)
{
    static_assert(std::is_integral<T>::value, "Return type must be an integral type.");

    if (byteOrder == ByteOrder::LITTLE_ENDIAN)
    {
        std::vector<unsigned char> reversedHexVector(hexVector.rbegin(), hexVector.rend());
        return hexVectorToInt<T>(reversedHexVector, ByteOrder::BIG_ENDIAN);
    }
    T result = 0;
    for (unsigned char byte : hexVector)
    {
        result = (result << 8) | byte;
    }

    return result;
}

template int hexVectorToInt<int>(const std::vector<unsigned char>, ByteOrder);
template long long hexVectorToInt<long long>(const std::vector<unsigned char>, ByteOrder);

template <typename T>
std::vector<unsigned char> intToHexVector(T value)
{
    static_assert(std::is_integral<T>::value, "Input type must be an integral type.");

    std::vector<unsigned char> hexVector;
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

// Creates a string "AB CD 45 " from a list of bytes, for example
inline std::string stringifyBytes(std::vector<unsigned char> bytes)
{
    std::ostringstream stream;
    for (auto val : bytes)
    {
        stream << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)val << " ";
    }
    std::string string = stream.str();
    string.pop_back();
    return string;
}

// Enum to represent different character encodings
enum class Encoding
{
    ASCII,
    UTF8,
    UTF16LE
};

// String to encoding
inline Encoding stringToEncoding(const std::string &encoding)
{
    if (encoding.find("ASCII") != std::string::npos || encoding.find("ANSI X3.4") != std::string::npos)
    {
        return Encoding::ASCII;
    }
    else if (encoding.find("UTF-8") != std::string::npos)
    {
        return Encoding::UTF8;
    }
    else if (encoding.find("UTF-16LE") != std::string::npos)
    {
        return Encoding::UTF16LE;
    }
    else
    {
        throw std::runtime_error("Unsupported encoding.");
    }
}

// Struct to hold a wstring and its encoding
struct EncodedString
{
    std::wstring str;
    Encoding encoding;
};

inline std::string convertToString(const std::vector<unsigned char> bytes)
{
    return std::string(bytes.begin(), bytes.end());
}

// Of note, wstring_convert is deprecated in C++17, function should later be updated to use https://icu.unicode.org libraries
inline std::wstring convertToWstring(const std::vector<unsigned char> bytes, Encoding encoding)
{
    std::string str(bytes.begin(), bytes.end());
    if (encoding == Encoding::ASCII || encoding == Encoding::UTF8)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.from_bytes(str);
    }
    else if (encoding == Encoding::UTF16LE)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t, 0x10ffff, std::codecvt_mode::little_endian>, char16_t> converter;
        std::u16string u16str = converter.from_bytes(str);
        return std::wstring(u16str.begin(), u16str.end());
    }
    else
    {
        throw std::runtime_error("Unsupported encoding.");
    }
};

inline EncodedString convertToEncodedString(const std::vector<unsigned char> bytes, Encoding encoding)
{
    EncodedString encodedString = {convertToWstring(bytes, encoding), encoding};
    return encodedString;
};

#endif