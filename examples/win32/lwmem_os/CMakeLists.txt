cmake_minimum_required(VERSION 3.22)

# Setup project
project(lwmem_multi_ins_multi_region)
add_executable(${PROJECT_NAME})

# Add key executable block
target_sources(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/main.c
)

# Add key include paths
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/

    # Snippets
    ${CMAKE_CURRENT_LIST_DIR}/../../../snippets/include
)

# Compilation definition information
target_compile_definitions(${PROJECT_NAME} PUBLIC
    WIN32
    _DEBUG
    CONSOLE
)

# Compiler options
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# Add subdir with lwmem and link to the project
set(LWMEM_SYS_PORT              "win32")
add_subdirectory("../../../lwmem" lwmem)
target_link_libraries(${PROJECT_NAME} lwmem)
