#include "NihonKohdenData.h"
#include "MFERData.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(monklib, m)
{
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
        .def_readonly("channels", &Header::channels);

    py::class_<NihonKohdenData>(m, "NihonKohdenData")
        .def(py::init<const string &>())
        .def("getHeader", &NihonKohdenData::getHeader,
             py::return_value_policy::copy, "Get the header of the data")
        .def("printData", &NihonKohdenData::printData, "Print the data in hex format")
        .def("printHeader", &NihonKohdenData::printHeader, "Print the header")
        .def("writeToCsv", &NihonKohdenData::writeToCsv);
}