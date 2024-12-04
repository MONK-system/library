#include "NihonKohdenData.h"
#include "MFERData.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>

namespace py = pybind11;

void printFileHeader(const std::string &filename)
{
    NihonKohdenData data(filename);
    data.printHeader();
}

void convertFileToCsv(const std::string &filename, const std::string &outputFilename)
{
    NihonKohdenData data(filename);
    data.writeToCsv(outputFilename);
}

Header getHeader(const std::string &filename)
{
    NihonKohdenData data(filename);
    return data.getHeader();
}

PYBIND11_MODULE(monklib, m)
{
    py::add_ostream_redirect(m, "ostream_redirect");

    py::enum_<ByteOrder>(m, "ByteOrder")
        .value("ENDIAN_BIG", ByteOrder::ENDIAN_BIG)
        .value("ENDIAN_LITTLE", ByteOrder::ENDIAN_LITTLE);

    py::class_<NIBPEvent>(m, "NIBPEvent")
        .def_readonly("eventCode", &NIBPEvent::eventCode)
        .def_readonly("startTime", &NIBPEvent::startTime)
        .def_readonly("duration", &NIBPEvent::duration)
        .def_readonly("information", &NIBPEvent::information);

    py::class_<Channel>(m, "Channel")
        .def_property_readonly("attribute", [](const Channel &c)
                               { return c.leadInfo.attribute; })
        .def_property_readonly("samplingResolution", [](const Channel &c)
                               { return c.leadInfo.samplingResolution; })
        .def_readonly("samplingInterval", &Channel::samplingIntervalString)
        .def_readonly("blockLength", &Channel::blockLength);

    py::class_<Header>(m, "Header")
        .def_readonly("preamble", &Header::preamble)
        .def_readonly("byteOrder", &Header::byteOrder)
        .def_readonly("modelInfo", &Header::modelInfo)
        .def_readonly("waveformType", &Header::waveformType)
        .def_readonly("measurementTimeISO", &Header::measurementTimeISO)
        .def_readonly("patientID", &Header::patientID)
        .def_readonly("patientName", &Header::patientName)
        .def_readonly("birthDateISO", &Header::birthDateISO)
        .def_readonly("patientSex", &Header::patientSex)
        .def_readonly("samplingInterval", &Header::samplingInterval)
        .def_readonly("samplingIntervalString", &Header::samplingIntervalString)
        .def_readonly("events", &Header::events)
        .def_readonly("sequenceCount", &Header::sequenceCount)
        .def_readonly("channelCount", &Header::channelCount)
        .def_readonly("channels", &Header::channels)
        .def("__str__", &Header::toString);

    py::class_<NihonKohdenData>(m, "Data")
        .def(py::init<const std::string &>())
        .def("getHeader", &NihonKohdenData::getHeader,
             py::return_value_policy::copy, "Get the header of the data")
        .def("anonymize", &NihonKohdenData::anonymize, "Anonymize the data")
        .def("setChannelSelection", &NihonKohdenData::setChannelSelection, "Set the channel selection at index to active")
        .def("setIntervalSelection", &NihonKohdenData::setIntervalSelection, "Set the interval selection in seconds (start = 0, end = 0). Setting end to 0 will select the remaining data")
        .def("writeToBinary", &NihonKohdenData::writeToBinary, "Write the data to a binary file")
        .def("writeToCsv", &NihonKohdenData::writeToCsv, "Write the data to a csv file");

    m.def("get_header", &getHeader, "Get the header of the file");
    m.def("print_header", &printFileHeader, "Print the header of the file");
    m.def("convert_to_csv", &convertFileToCsv, "Convert the file to a csv file");
}
