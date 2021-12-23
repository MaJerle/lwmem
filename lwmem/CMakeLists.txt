cmake_minimum_required(VERSION 3.0.0)

# Check definitions of some key things
if(NOT DEFINED LWMEM_SYS_ARCH)
    message("Warning: LWMEM_SYS_ARCH not defined. Using default one as \"win32\"")
    set(LWMEM_SYS_ARCH "win32")
endif()

# Add source files
target_sources(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/lwmem/lwmem.c
    ${CMAKE_CURRENT_LIST_DIR}/src/system/lwmem_sys_${LWMEM_SYS_ARCH}.c
    )

# Add include files
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/include
    )
