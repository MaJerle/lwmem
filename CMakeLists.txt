cmake_minimum_required(VERSION 3.0.0)

# Setup project
project(LwLibPROJECT VERSION 0.1.0)

# Add executable and library
add_executable(${PROJECT_NAME})

# Add key executable block
target_sources(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/tests/lwmem_test.c
    ${CMAKE_CURRENT_LIST_DIR}/dev/VisualStudio/main.c
    )

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/dev/VisualStudio
    )

target_compile_definitions(${PROJECT_NAME} PRIVATE
    WIN32
    _DEBUG
    CONSOLE
    LWMEM_DEV
    )

# Go to sub directory
add_subdirectory(lwmem)
