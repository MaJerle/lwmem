cmake_minimum_required(VERSION 3.13)

# Debug message
message("LWMEM: Entering lib source CMakeLists.txt")

# Set library name
set(LWMEM_LIB_NAME "lwmem")

# Check definitions of some key things
option(LWMEM_SYS_ADD_FILES OFF)
if (LWMEM_SYS_ADD_FILES)
    message("LWMEM: LWMEM_SYS_ADD_FILES is set to ON. Looking for LWMEM_SYS_ARCH definition")
    if(NOT DEFINED LWMEM_SYS_ARCH)
        message("LWMEM: LWMEM_SYS_ARCH not defined. Using default one: \"win32\"")
        set(LWMEM_SYS_ARCH "win32")
    endif()
    message("LWMEM: LWPRINTF_SYS_ARCH set to: " ${LWMEM_SYS_ARCH})
else()
    message("LWMEM: LWMEM_SYS_ADD_FILES is set to OFF. User must manually implement system functions in case of operating system support")
endif()

# Register library to the system
add_library(${LWMEM_LIB_NAME} INTERFACE)

# Setup generic source files
target_sources(${LWMEM_LIB_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/lwmem/lwmem.c
    )

# Add optional sources
if (LWMEM_SYS_ADD_FILES)
target_sources(${LWMEM_LIB_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/system/lwmem_sys_${LWMEM_SYS_ARCH}.c
    )
endif()

# Setup include directories
target_include_directories(${LWMEM_LIB_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/include
    )
