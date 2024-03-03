cmake_minimum_required(VERSION 3.0)
project(Library)

# Add source files here
set(SOURCES
    main.cpp
    MFERData.cpp
    MFERDataCollection.cpp
    DataStack.cpp
    FileManager.cpp
    NihonKohdenData.cpp
    ByteVector.cpp
)

# Add header file here
set(HEADERS
    ByteVector.h
    DataStack.h
    FileManager.h
    MFERData.h
    MFERDataCollection.h
    NihonKohdenData.h
)

# Create library target
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

# Create executable target
add_executable(${PROJECT_NAME}_app main.cpp)

# Link library to the executable
target_link_libraries(${PROJECT_NAME}_app ${PROJECT_NAME})

# Specify include directories
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
