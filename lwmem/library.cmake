#
# LIB_PREFIX: LWMEM
#
# This file provides set of variables for end user
# and also generates one (or more) libraries, that can be added to the project using target_link_libraries(...)
#
# Before this file is included to the root CMakeLists file (using include() function), user can set some variables:
#
# LWMEM_SYS_PORT: If defined, it will include port source file from the library.
# LWMEM_OPTS_FILE: If defined, it is the path to the user options file. If not defined, one will be generated for you automatically
# LWMEM_COMPILE_OPTIONS: If defined, it provide compiler options for generated library.
# LWMEM_COMPILE_DEFINITIONS: If defined, it provides "-D" definitions to the library build
#

# Custom include directory
set(LWMEM_CUSTOM_INC_DIR ${CMAKE_CURRENT_BINARY_DIR}/lib_inc)

# Library core sources
set(lwmem_core_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/src/lwmem/lwmem.c
)

# C++ extension
set(lwmem_core_cpp_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/src/lwmem/lwmem.cpp
)

# Setup include directories
set(lwmem_include_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/src/include
    ${LWMEM_CUSTOM_INC_DIR}
)

# Add system port
if(DEFINED LWMEM_SYS_PORT)
    set(lwmem_core_SRCS ${lwmem_core_SRCS}
        ${CMAKE_CURRENT_LIST_DIR}/src/system/lwmem_sys_${LWMEM_SYS_PORT}.c
    )
endif()

# Register core library
add_library(lwmem)
target_sources(lwmem PRIVATE ${lwmem_core_SRCS})
target_include_directories(lwmem PUBLIC ${lwmem_include_DIRS})
target_compile_options(lwmem PRIVATE ${LWMEM_COMPILE_OPTIONS})
target_compile_definitions(lwmem PRIVATE ${LWMEM_COMPILE_DEFINITIONS})

# Register core library with C++ extensions
add_library(lwmem_cpp)
target_sources(lwmem_cpp PRIVATE ${lwmem_core_SRCS})
target_include_directories(lwmem_cpp PUBLIC ${lwmem_include_DIRS})
target_compile_options(lwmem_cpp PRIVATE ${LWMEM_COMPILE_OPTIONS})
target_compile_definitions(lwmem_cpp PRIVATE ${LWMEM_COMPILE_DEFINITIONS})
target_link_libraries(lwmem_cpp PUBLIC lwmem)

# Create config file if user didn't provide one info himself
if(NOT LWMEM_OPTS_FILE)
    message(STATUS "Using default lwmem_opts.h file")
    set(LWMEM_OPTS_FILE ${CMAKE_CURRENT_LIST_DIR}/src/include/lwmem/lwmem_opts_template.h)
else()
    message(STATUS "Using custom lwmem_opts.h file from ${LWMEM_OPTS_FILE}")
endif()

configure_file(${LWMEM_OPTS_FILE} ${LWMEM_CUSTOM_INC_DIR}/lwmem_opts.h COPYONLY)
