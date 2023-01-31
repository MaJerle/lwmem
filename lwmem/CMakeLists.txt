cmake_minimum_required(VERSION 3.22)

# Debug message
message("Entering ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")

# Register core library
add_library(lwmem INTERFACE)
target_sources(lwmem PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src/lwmem/lwmem.c)
target_include_directories(lwmem INTERFACE ${CMAKE_CURRENT_LIST_DIR}/src/include)

if (DEFINED LWMEM_SYS_PORT)
target_sources(lwmem PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src/system/lwmem_sys_${LWMEM_SYS_PORT}.c)
endif()

# Register core library with C++ extensions
add_library(lwmem_cpp INTERFACE)
target_sources(lwmem_cpp PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src/lwmem/lwmem.cpp)
target_include_directories(lwmem_cpp INTERFACE ${CMAKE_CURRENT_LIST_DIR}/src/include)

# Register other modules

# Debug message
message("Exiting ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")
