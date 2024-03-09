cmake_minimum_required(VERSION 3.5...3.26)
project(monkLib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ version selection")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include pybind11
add_subdirectory(pybind11)

# Add source files here
set(SOURCES
    pyMONK.cpp
    MFERData.cpp
    MFERDataCollection.cpp
    DataStack.cpp
    FileManager.cpp
    NihonKohdenData.cpp
    ByteVector.cpp
)

# Add header files here
set(HEADERS
    ByteVector.h
    DataStack.h
    FileManager.h
    MFERData.h
    MFERDataCollection.h
    NihonKohdenData.h
)

# Define module
pybind11_add_module(${PROJECT_NAME} MODULE ${SOURCES} ${HEADERS})

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_link_libraries(${PROJECT_NAME} PRIVATE pybind11::module)
