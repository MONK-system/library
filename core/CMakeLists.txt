file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.h")

add_library(corelib STATIC ${SOURCES} ${HEADERS})
target_link_libraries(corelib PUBLIC pybind11::pybind11)

target_include_directories(corelib PUBLIC include)