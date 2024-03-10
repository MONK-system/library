cmake_minimum_required(VERSION 3.5...3.26)
project(monkLib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ version selection")

# Include pybind11
add_subdirectory(pybind11)

set(SOURCES
    src/pybind.cpp
    src/MFERData.cpp
    src/MFERDataCollection.cpp
    src/DataStack.cpp
    src/FileManager.cpp
    src/NihonKohdenData.cpp
    src/ByteVector.cpp
)

set(HEADERS
    src/ByteVector.h
    src/DataStack.h
    src/FileManager.h
    src/MFERData.h
    src/MFERDataCollection.h
    src/NihonKohdenData.h
)

# Define module
pybind11_add_module(${PROJECT_NAME} MODULE ${SOURCES} ${HEADERS})

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_link_libraries(${PROJECT_NAME} PRIVATE pybind11::module)
