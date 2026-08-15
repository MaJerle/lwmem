# CMake include file

# Add more sources
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/lwmem_test_os.c
)

# Options file
set(LWMEM_OPTS_FILE ${CMAKE_CURRENT_LIST_DIR}/lwmem_opts.h)

# Only this test case exercises LWMEM_CFG_OS -- build & link the system mutex port for it
set(LWMEM_TEST_NEEDS_OS_PORT TRUE)
