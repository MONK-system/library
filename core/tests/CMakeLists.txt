# Add test files
file(GLOB TEST_SOURCES "*_test.cpp")
foreach(TEST_FILE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} gtest_main gtest corelib)
    gtest_discover_tests(${TEST_NAME} DISCOVERY_MODE PRE_TEST)
endforeach()