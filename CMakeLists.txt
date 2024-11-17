cmake_minimum_required(VERSION 3.8...3.18)
project(monklib)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ version selection")

set(CMAKE_BUILD_WITH_INSTALL_RPATH ON) # Use RPATH for installed targets during build

set(CMAKE_POSITION_INDEPENDENT_CODE ON) # Enable position independent code

enable_testing() # Enable testing

# Fetch Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable Google Test macros
include(GoogleTest)

# Define the test data file
set(TEST_DATA_FILE "${CMAKE_CURRENT_SOURCE_DIR}/resources/test-file.MWF")

# Include pybind11
add_subdirectory(pybind11)

# Include core library
add_subdirectory(core)

# Define module
pybind11_add_module(${PROJECT_NAME} bindings/pybind.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE corelib)