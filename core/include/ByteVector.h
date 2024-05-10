#ifndef BINARYVECTOR_H
#define BINARYVECTOR_H

#include <string>
#include <vector>
#include <iomanip>
#include <cstdint>
#include <cstring>

enum class ByteOrder
{
    ENDIAN_BIG,
    ENDIAN_LITTLE
};

// Enum to represent different character encodings
enum class Encoding
{
    ASCII,
    UTF8,
    UTF16LE
};

// String to encoding
Encoding stringToEncoding(const std::string &encoding);

// ByteVector class to represent a vector of bytes
class ByteVector : public std::vector<uint8_t>
{
public:
    using vector::vector;
    std::string stringify() const;                                   // Creates a string "AB CD 45 " from a list of bytes, for example
    std::string toString(Encoding encoding = Encoding::ASCII) const; // Converts the byte vector to a string using the ASCII encoding by default

    template <typename T>
    inline T toInt(ByteOrder byteOrder = ByteOrder::ENDIAN_BIG) const
    {
        static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value, "Return type must be an integral or floating-point type.");
        if (this->size() > sizeof(T))
        {
            throw std::runtime_error("Vector size is larger than the size of the return type.");
        }

        T value;
        // If the byte order needs to be reversed for this system, reverse the vector before copying.
        if (byteOrder == ByteOrder::ENDIAN_BIG)
        {
            // Systems are typically little-endian; reverse if original data is big-endian.
            ByteVector reversedByteVector(this->rbegin(), this->rend());
            memcpy(&value, reversedByteVector.data(), sizeof(T));
        }
        else
        {
            // If the system's byte order matches the data's byte order, copy directly.
            memcpy(&value, this->data(), sizeof(T));
        }

        return value;
    }

    template <typename T>
    inline static ByteVector fromInt(T value)
    {
        static_assert(std::is_integral<T>::value, "Input type must be an integral type.");

        ByteVector byteVector;
        bool leadingZero = true;

        for (int i = (sizeof(T) - 1) * 8; i >= 0; i -= 8)
        {
            uint8_t byte = (value >> i) & 0xFF;
            if (byte != 0 || !leadingZero)
            {
                byteVector.push_back(byte);
                leadingZero = false;
            }
        }

        // In case the value is 0, ensure the vector is not empty
        if (byteVector.empty())
        {
            byteVector.push_back(0);
        }

        return byteVector;
    }

    void initializeInterpreter() const;
    void finalizeInterpreter() const;

private:
    inline static int initializeCount = 0;
};

class InterpreterGuard
{
public:
    InterpreterGuard(ByteVector *ptr) : ptr_(ptr) {}
    ~InterpreterGuard() { ptr_->finalizeInterpreter(); }

private:
    ByteVector *ptr_;
};

#endif
