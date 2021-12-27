cmake_minimum_required(VERSION 3.13)

# Debug message
message("Entering ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")

# Register core library
add_library(lwmem INTERFACE)
target_sources(lwmem PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src/lwmem/lwmem.c)
target_include_directories(lwmem INTERFACE ${CMAKE_CURRENT_LIST_DIR}/src/include)

# Register other modules

# Debug message
message("Exiting ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")
