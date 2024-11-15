# Add test files
file(GLOB TEST_SOURCES "*_test.cpp")

foreach(TEST_FILE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} gtest_main gtest corelib)

    # Ensure the test data file is copied to the build directory
    add_custom_command(
        TARGET ${TEST_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${TEST_DATA_FILE} $<TARGET_FILE_DIR:${TEST_NAME}>
    )

    # Register the test with CTest
    gtest_discover_tests(${TEST_NAME} WORKING_DIRECTORY $<TARGET_FILE_DIR:${TEST_NAME}>)
endforeach()
