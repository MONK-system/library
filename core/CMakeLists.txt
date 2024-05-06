# Get all source files
file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.h")

# Add the library
add_library(corelib STATIC ${SOURCES} ${HEADERS})
target_link_libraries(corelib PUBLIC pybind11::module pybind11::embed) # Link to pybind11

target_include_directories(corelib PUBLIC include) # Include the headers

add_subdirectory(tests) # Add the tests