cmake_minimum_required(VERSION 3.13)

# Debug message
message("LWMEM: Entering lib source CMakeLists.txt")

# Set library name
set(LWMEM_LIB_NAME "lwmem")

# Check definitions of some key things
if(NOT DEFINED LWMEM_SYS_ARCH)
    message("LWMEM: LWMEM_SYS_ARCH not defined. Using default one: \"win32\"")
    set(LWMEM_SYS_ARCH "win32")
endif()

# Register library to the system
add_library(${LWMEM_LIB_NAME} INTERFACE)

# Setup generic source files
target_sources(${LWMEM_LIB_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/lwmem/lwmem.c
    ${CMAKE_CURRENT_LIST_DIR}/src/system/lwmem_sys_${LWMEM_SYS_ARCH}.c
    )

# Setup include directories
target_include_directories(${LWMEM_LIB_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/include
    )