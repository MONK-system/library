#include "ByteVector.h"
#include <pybind11/embed.h>
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
    InterpreterGuard guard; // Initialize embedded python interpreter and finalize when out of scope

    py::gil_scoped_acquire acquire; // Ensure GIL is held

    try
    {
        std::string str(this->begin(), this->end());
        if (encoding == Encoding::ASCII || encoding == Encoding::UTF8)
        {
            return py::bytes(str).attr("decode")(py::str("utf-8")).cast<std::string>();
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
    catch (const py::error_already_set &e)
    {
        throw std::runtime_error("Python error occurred in ByteVector::toString");
    }
}

void InterpreterGuard::initializeInterpreter() const
{
    if (initializeCount++ == 0) // If first initialization
    {
        if (Py_IsInitialized() == 0) // If Python interpreter is not initialized
        {
            py::initialize_interpreter(); // Initialize Python interpreter
        }
        else // If Python interpreter is already initialized
        {
            initializeCount++; // Increment the count to avoid re-initialization
        }
    }
}

void InterpreterGuard::finalizeInterpreter() const
{
    if (--initializeCount == 0) // If last finalization
    {
        if (Py_IsInitialized() != 0) // If Python interpreter is initialized
        {
            py::finalize_interpreter(); // Finalize Python interpreter
        }
    }
}
