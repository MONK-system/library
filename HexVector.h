#ifndef HEXVECTOR_H
#define HEXVECTOR_H

#include <vector>
#include <type_traits>
#include <sstream>
#include <iomanip>

template <typename T>
T hexVectorToInt(const std::vector<unsigned char> &hexVector)
{
    static_assert(std::is_integral<T>::value, "Return type must be an integral type.");

    T result = 0;
    for (unsigned char byte : hexVector)
    {
        result = (result << 8) | byte;
    }

    return result;
}

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

#endif