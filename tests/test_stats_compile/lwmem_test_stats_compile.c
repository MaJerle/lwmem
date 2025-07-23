/**
 * \file            lwmem_test_simple.c
 * \author          Tilen MAJERLE <tilen@majerle.eu>
 * \brief           
 * \version         0.1
 * \date            2025-03-30
 * 
 * @copyright Copyright (c) 2025
 * 
 * Test the compile for stats + full config
 */
#include <stdio.h>
#include "lwmem/lwmem.h"
#include "test.h"

/********************************************/
/* Region memory declaration */
/* Use uint32 for alignment reasons */
static uint32_t lw_c_mem1[64 / 4];

/* Regions descriptor */
static lwmem_region_t lw_c_regions[] = {
    {lw_c_mem1, sizeof(lw_c_mem1)},
    {NULL, 0},
};

/********************************************/

int
test_run(void) {
    size_t retval;
    void* ptr;

    /* Should fly now */
    retval = lwmem_assignmem(lw_c_regions);
    TEST_ASSERT(retval != 0);

    /* We have 64 bytes from now on */

    /* Try to allocate memory */
    ptr = lwmem_malloc(32);
    TEST_ASSERT(ptr != NULL);
    ptr = lwmem_malloc(32);
    TEST_ASSERT(ptr == NULL);
    ptr = lwmem_malloc(4);
    TEST_ASSERT(ptr != NULL);

    return 0;
}
