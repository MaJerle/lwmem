cmake_minimum_required(VERSION 3.0.0)

# Setup project
project(LwMEM VERSION 2.0.1)

# ------------------------
# Configuration used if this is used as
# top-level project CMakeLists.txt file
# This is normally used for development purpose only
if (PROJECT_IS_TOP_LEVEL)
    add_executable(${PROJECT_NAME})

    # Add key executable block
    target_sources(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/dev/VisualStudio/main.c
        ${CMAKE_CURRENT_LIST_DIR}/tests/lwmem_test.c
        )

    target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/dev/VisualStudio
        )

    target_compile_definitions(${PROJECT_NAME} PUBLIC
        WIN32
        _DEBUG
        CONSOLE
        LWMEM_DEV
        )
endif()

# Add subdir with lwmem and link to the project
add_subdirectory("lwmem")
target_link_libraries(${PROJECT_NAME} lwmem)