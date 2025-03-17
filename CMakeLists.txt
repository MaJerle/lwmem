cmake_minimum_required(VERSION 3.22)

# Setup project
project(LwLibPROJECT)

if(NOT PROJECT_IS_TOP_LEVEL)
    add_subdirectory(lwmem)
else()
    # Set default compile flags for GCC
    if(CMAKE_COMPILER_IS_GNUCXX)
        message(STATUS "GCC detected, adding compile flags")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra")
    endif(CMAKE_COMPILER_IS_GNUCXX)

    enable_testing()
    add_executable(${PROJECT_NAME})
    target_sources(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/dev/main.cpp
        ${CMAKE_CURRENT_LIST_DIR}/tests/lwmem_test.c
        ${CMAKE_CURRENT_LIST_DIR}/tests/lwmem_test_simple.c
        ${CMAKE_CURRENT_LIST_DIR}/tests/lwmem_test_region.c
        ${CMAKE_CURRENT_LIST_DIR}/tests/lwmem_test_available_mem.c

        # win32 port
        ${CMAKE_CURRENT_LIST_DIR}/lwmem/src/system/lwmem_sys_win32.c
    )
    target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/dev
        ${CMAKE_CURRENT_LIST_DIR}/tests
    )

    # Add subdir with lwmem and link to the project
    set(LWMEM_OPTS_FILE ${CMAKE_CURRENT_LIST_DIR}/dev/lwmem_opts.h)
    add_subdirectory(lwmem)
    target_link_libraries(${PROJECT_NAME} PUBLIC lwmem_cpp)

    # Add compile options to the library, which will propagate options to executable through public link
    target_compile_definitions(lwmem PUBLIC WIN32 _DEBUG CONSOLE LWMEM_DEV)
    target_compile_options(lwmem PUBLIC -Wall -Wextra -Wpedantic)

    # Add test
    add_test(NAME Test COMMAND $<TARGET_FILE:${CMAKE_PROJECT_NAME}>)
endif()
