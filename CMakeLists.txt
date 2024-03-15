cmake_minimum_required(VERSION 3.8...3.18)
project(monklib)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ version selection")

# Include pybind11
add_subdirectory(pybind11)

file(GLOB SOURCES "src/*.cpp")

file(GLOB HEADERS "src/*.h")

# Define module
pybind11_add_module(${PROJECT_NAME} ${SOURCES} ${HEADERS})
