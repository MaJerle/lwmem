cmake_minimum_required(VERSION 3.22)

# Setup project
project(LwLibPROJECT)

if(NOT PROJECT_IS_TOP_LEVEL)
    add_subdirectory(lwmem)
else()
    set(TEST_CMAKE_FILE_NAME "test_simple_config/cmake.cmake")
    add_subdirectory(tests)
endif()
