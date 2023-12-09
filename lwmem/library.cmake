# 
# This file provides set of variables for end user
# and also generates one (or more) libraries, that can be added to the project using target_link_libraries(...)
#
# Before this file is included to the root CMakeLists file (using include() function), user can set some variables:
#
# LWMEM_SYS_PORT: If defined, it will include port source file from the library.
# LWMEM_OPTS_DIR: If defined, it should set the folder path where options file shall be generated.
# LWMEM_COMPILE_OPTIONS: If defined, it provide compiler options for generated library.
# LWMEM_COMPILE_DEFINITIONS: If defined, it provides "-D" definitions to the library build
#

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
)

# Add system port
if(DEFINED LWMEM_SYS_PORT)
set(lwmem_core_SRCS ${lwmem_core_SRCS}
    ${CMAKE_CURRENT_LIST_DIR}/src/system/lwmem_sys_${LWMEM_SYS_PORT}.c
)
endif()

# Register core library
add_library(lwmem INTERFACE)
target_sources(lwmem PUBLIC ${lwmem_core_SRCS})
target_include_directories(lwmem INTERFACE ${lwmem_include_DIRS})
target_compile_options(lwmem PRIVATE ${LWMEM_COMPILE_OPTIONS})
target_compile_definitions(lwmem PRIVATE ${LWMEM_COMPILE_DEFINITIONS})

# Register core library with C++ extensions
add_library(lwmem_cpp INTERFACE)
target_sources(lwmem_cpp PUBLIC ${lwmem_core_SRCS})
target_include_directories(lwmem_cpp INTERFACE ${lwmem_include_DIRS})
target_compile_options(lwmem_cpp PRIVATE ${LWMEM_COMPILE_OPTIONS})
target_compile_definitions(lwmem_cpp PRIVATE ${LWMEM_COMPILE_DEFINITIONS})

# Create config file
if(DEFINED LWMEM_OPTS_DIR AND NOT EXISTS ${LWMEM_OPTS_DIR}/lwmem_opts.h)
    configure_file(${CMAKE_CURRENT_LIST_DIR}/src/include/lwmem/lwmem_opts_template.h ${LWMEM_OPTS_DIR}/lwmem_opts.h COPYONLY)
endif()
