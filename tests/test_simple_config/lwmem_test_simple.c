/**
 * \file            lwmem_test_simple.c
 * \author          Tilen MAJERLE <tilen@majerle.eu>
 * \brief           
 * \version         0.1
 * \date            2025-03-30
 * 
 * @copyright Copyright (c) 2025
 * 
 * Test code for simple configuration, a LwMEM config that does
 * not support any memory free operation (a lite mode)
 */
#include <stdio.h>
#include "lwmem/lwmem.h"
#include "test.h"

#if !LWMEM_CFG_FULL

/********************************************/
/* Configuration for default lwmem instance */

/* Region memory declaration */
static struct {
    uint8_t m1[128];
    uint8_t m2[256];
    uint8_t m3[1024];
} lw_mem;

/* Regions descriptor */
static lwmem_region_t lw_regions_too_many[] = {
    {lw_mem.m1, sizeof(lw_mem.m1)},
    {lw_mem.m2, sizeof(lw_mem.m2)},
    {lw_mem.m3, sizeof(lw_mem.m3)},
    {NULL, 0},
};

/********************************************/
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

    /* Should fail -> too many regions */
    retval = lwmem_assignmem(lw_regions_too_many);
    TEST_ASSERT(retval == 0);

    /* Should fly now */
    retval = lwmem_assignmem(lw_c_regions);
    TEST_ASSERT(retval != 0);

    /* We have 64 bytes from now on */

    /* Try to allocate memory */
    ptr = lwmem_malloc(32);
    TEST_ASSERT(ptr != NULL);
    ptr = lwmem_malloc(32);
    TEST_ASSERT(ptr != NULL);
    ptr = lwmem_malloc(4);
    TEST_ASSERT(ptr == NULL);

    return 0;
}

#endif /* !LWMEM_CFG_FULL */