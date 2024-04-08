#include "ByteVector.h"
#include <codecvt>
#include <cstdint>
#include <vector>
#include <sstream>
#include <iostream>

std::string ByteVector::stringify() const
{
    std::ostringstream stream;
    for (auto val : *this)
    {
        stream << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)val << " ";
    }
    std::string string = stream.str();
    string.pop_back();
    return string;
}

// Of note, wstring_convert is deprecated in C++17, function should later be updated to use https://icu.unicode.org libraries
std::string ByteVector::toString(Encoding encoding) const
{
    try
    {
        std::string str(this->begin(), this->end());
        if (encoding == Encoding::ASCII || encoding == Encoding::UTF8)
        {
            return str;
        }
        else if (encoding == Encoding::UTF16LE)
        {
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t, 0x10ffff, std::codecvt_mode::little_endian>, char16_t> converter;
            std::u16string u16str = converter.from_bytes(str);
            return converter.to_bytes(u16str);
        }
        else
        {
            throw std::runtime_error("Unsupported encoding.");
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception occurred in toString: " << e.what() << '\n';
    }
}

Encoding stringToEncoding(const std::string &encoding)
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
