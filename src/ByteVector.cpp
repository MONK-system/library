#include "ByteVector.h"
#include <pybind11/pybind11.h>
#include <cstdint>
#include <vector>
#include <sstream>
#include <iostream>

namespace py = pybind11;

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
            return py::bytes(str).attr("decode")(py::str("utf-16-le")).cast<std::string>();
        }
        else
        {
            throw std::runtime_error("Unsupported encoding.");
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception occurred in toString: " << e.what() << '\n';
        return "ERROR";
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
