cmake_minimum_required(VERSION 3.22)

# Setup project
project(LwLibPROJECT)

if(NOT PROJECT_IS_TOP_LEVEL)
    add_subdirectory(lwmem)
else()
    add_executable(${PROJECT_NAME})
    target_sources(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/dev/main.cpp
        ${CMAKE_CURRENT_LIST_DIR}/tests/lwmem_test.c

        # win32 port
        ${CMAKE_CURRENT_LIST_DIR}/lwmem/src/system/lwmem_sys_win32.c
    )
    target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/dev
    )

    # Add subdir with lwmem and link to the project
    set(LWMEM_OPTS_FILE ${CMAKE_CURRENT_LIST_DIR}/dev/lwmem_opts.h)
    add_subdirectory(lwmem)
    target_link_libraries(${PROJECT_NAME} PUBLIC lwmem_cpp)

    # Add compile options to the library, which will propagate options to executable through public link
    target_compile_definitions(lwmem PUBLIC WIN32 _DEBUG CONSOLE LWMEM_DEV)
    target_compile_options(lwmem PUBLIC -Wall -Wextra -Wpedantic)
endif()
