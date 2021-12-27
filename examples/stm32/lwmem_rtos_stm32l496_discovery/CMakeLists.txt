cmake_minimum_required(VERSION 3.12)

# ##############################################################################
# Edit your MCU information up to the next border
project(lwmem_l496vg)
set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(MCU_FAMILY              STM32L4xx)
set(MCU_MODEL               STM32L496xx)
set(CPU_PARAMETERS
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    )

# Set startup and linker script
set(STARTUP_SCRIPT      ${CMAKE_CURRENT_SOURCE_DIR}/STM32CubeIDE/startup/startup_stm32l496xx.s)
set(MCU_LINKER_SCRIPT   ${CMAKE_CURRENT_SOURCE_DIR}/STM32CubeIDE/Debug_STM32L496AG_FLASH.ld)

# ##############################################################################
set(EXECUTABLE                  ${CMAKE_PROJECT_NAME})
enable_language(C CXX ASM)
set(CMAKE_C_STANDARD            11)
set(CMAKE_C_STANDARD_REQUIRED   ON)
set(CMAKE_C_EXTENSIONS          ON)
set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        ON)

# Include headers
set(st_cube_include_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/st/${MCU_FAMILY}_HAL_Driver/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/st/${MCU_FAMILY}_HAL_Driver/Inc/Legacy
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/st/CMSIS/Device/ST/${MCU_FAMILY}/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/st/CMSIS/include
    )
set(project_include_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    )
set(freertos_include_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/FreeRTOS/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/FreeRTOS/CMSIS_RTOS_V2
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/FreeRTOS/portable/GCC/ARM_CM4F
    )

# Source files
set(st_cube_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_dma.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_exti.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_gpio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_pwr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_rcc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_usart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_utils.c
    )
set(project_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/stm32l4xx_it.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/syscalls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/system_stm32l4xx.c

    # LwMEM system func
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../lwmem/src/system/lwmem_sys_cmsis_os.c    
    )
set(freertos_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/FreeRTOS/CMSIS_RTOS_V2/cmsis_os2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/FreeRTOS/croutine.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/FreeRTOS/event_groups.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/FreeRTOS/portable/MemMang/heap_4.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/FreeRTOS/list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/FreeRTOS/portable/GCC/ARM_CM4F/port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/FreeRTOS/queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/FreeRTOS/tasks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/FreeRTOS/timers.c
    )

# Executable files
add_executable(${EXECUTABLE}
    ${st_cube_SRCS} 
    ${project_SRCS}
    ${freertos_SRCS}
    ${STARTUP_SCRIPT}
    )

# Embedded macros(defines)
target_compile_definitions(${EXECUTABLE} PRIVATE
    "__weak=__attribute__((weak))"
    "__packed=__attribute__((__packed__))"
    ${MCU_MODEL}
    USE_FULL_LL_DRIVER
    )

# Add header directories (AFTER add_executable !!)
target_include_directories(${EXECUTABLE} PRIVATE
    ${st_cube_include_DIRS}
    ${project_include_DIRS}
    ${freertos_include_DIRS}
    )

# Add lwmem to the project
add_subdirectory("../../../lwmem" lwmem)
target_link_libraries(${EXECUTABLE} lwmem)

# Compiler and linker options
target_compile_options(${EXECUTABLE} PRIVATE
    ${CPU_PARAMETERS}
    -Wall
    -Wextra
    -Wpedantic
    -Wno-unused-parameter
    $<$<COMPILE_LANGUAGE:CXX>:
        -Wno-volatile
        -Wold-style-cast
        -Wuseless-cast
        -Wsuggest-override>
    $<$<CONFIG:Debug>:-Og -g3 -ggdb>
    $<$<CONFIG:Release>:-Og -g0>
    )

# Setup linker parameters
target_link_options(${EXECUTABLE} PRIVATE
    -T${MCU_LINKER_SCRIPT}
    ${CPU_PARAMETERS}
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map
    --specs=nosys.specs
    -Wl,--start-group
    -lc
    -lm
    -lstdc++
    -lsupc++
    -Wl,--end-group
    -Wl,--print-memory-usage)

# Execute post-build to print size
add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${EXECUTABLE}>)

# Convert output to hex and binary
add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${EXECUTABLE}> ${EXECUTABLE}.hex)

# Conver to bin file -> add conditional check?
add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${EXECUTABLE}> ${EXECUTABLE}.bin)