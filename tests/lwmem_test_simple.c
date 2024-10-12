#include <stdio.h>
#include "lwmem/lwmem.h"

#if !LWMEM_CFG_FULL

/* Assert check */
#define ASSERT(x)                                                                                                      \
    do {                                                                                                               \
        if (!(x)) {                                                                                                    \
            printf("Assert on line %d failed with condition (" #x ")\r\n", (int)__LINE__);                             \
        } else {                                                                                                       \
            printf("Assert on line %d passed with condition (" #x ")\r\n", (int)__LINE__);                             \
        }                                                                                                              \
    } while (0)

/********************************************/
/* Configuration for default lwmem instance */

/* Region memory declaration */
static uint8_t lw_mem1[1024], lw_mem2[256], lw_mem3[128];

/* Regions descriptor */
static lwmem_region_t lw_regions_too_many[] = {
    {lw_mem3, sizeof(lw_mem3)},
    {lw_mem2, sizeof(lw_mem2)},
    {lw_mem1, sizeof(lw_mem1)},
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

void
lwmem_test_simple_run(void) {
    size_t retval;
    void* ptr;

    /* Should fail -> too many regions */
    retval = lwmem_assignmem(lw_regions_too_many);
    ASSERT(retval == 0);

    /* Should fly now */
    retval = lwmem_assignmem(lw_c_regions);
    ASSERT(retval != 0);

    /* We have 64 bytes from now on */

    /* Try to allocate memory */
    ptr = lwmem_malloc(32);
    ASSERT(ptr != NULL);
    ptr = lwmem_malloc(32);
    ASSERT(ptr != NULL);
    ptr = lwmem_malloc(4);
    ASSERT(ptr == NULL);
}

#endif /* !LWMEM_CFG_FULL */