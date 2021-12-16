cmake_minimum_required(VERSION 3.0.0)
project(LwLibPROJECT VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(${PROJECT_NAME}
    lwmem/src/lwmem/lwmem.c
    lwmem/src/system/lwmem_sys_win32.c
    tests/lwmem_test.c
    dev/VisualStudio/main.c
    )

target_include_directories(${PROJECT_NAME} PRIVATE
    dev/VisualStudio
    lwmem/src/include
    )

target_compile_definitions(${PROJECT_NAME} PRIVATE
    WIN32
    _DEBUG
    CONSOLE
    LWMEM_DEV
    )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
