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
