#include <stdio.h>
#include "lwmem/lwmem.h"
#include "test.h"

#if LWMEM_CFG_FULL

/********************************************/
/* Test case helpers                        */
#define UINT_PTR_CAST(x) ((uintptr_t)(x))
#define IS_ALLOC_IN_REGION(ptr, region)                                                                                \
    TEST_ASSERT(UINT_PTR_CAST(ptr) >= UINT_PTR_CAST((region)->start_addr)                                              \
                && UINT_PTR_CAST(ptr) < (UINT_PTR_CAST((region)->start_addr) + (region)->size))

/********************************************/
/* Configuration for default lwmem instance */

/* Region memory declaration */
static struct {
    uint8_t m1[128];
    uint8_t m2[256];
    uint8_t m3[1024];
} lw_mem;

/* Regions descriptor */
static lwmem_region_t lw_regions[] = {
    {lw_mem.m1, sizeof(lw_mem.m1)},
    {lw_mem.m2, sizeof(lw_mem.m2)},
    {lw_mem.m3, sizeof(lw_mem.m3)},
    {NULL, 0},
};

/********************************************/
/********************************************/
/* Configuration for custom lwmem instance  */
/* LwMEM instance */
static lwmem_t lw_c;

static struct {
    uint8_t m1[128];
    uint8_t m2[256];
    uint8_t m3[1024];
} lw_mem_c;

/* Regions descriptor */
static lwmem_region_t lw_c_regions[] = {
    {lw_mem_c.m1, sizeof(lw_mem_c.m1)},
    {lw_mem_c.m2, sizeof(lw_mem_c.m2)},
    {lw_mem_c.m3, sizeof(lw_mem_c.m3)},
    {NULL, 0},
};

/********************************************/

int
test_run(void) {
    void *ptr_1 = NULL, *ptr_2 = NULL, *ptr_3 = NULL;
    void *ptr_c_1 = NULL, *ptr_c_2 = NULL, *ptr_c_3 = NULL;

    /* Initialize default lwmem instance */
    /* Use one of 2 possible function calls: */
    lwmem_assignmem(lw_regions);
    //lwmem_assignmem_ex(NULL, lw_regions);

    /* Regions initialized... */

    /********************************************/
    /* Run tests on default region              */
    /********************************************/

    /* Allocation of 64 bytes must in in first region */
    ptr_1 = lwmem_malloc(64);
    TEST_ASSERT(ptr_1 != NULL);
    IS_ALLOC_IN_REGION(ptr_1, &lw_regions[0]);

    /* Allocation of 256 bytes can only be in 3rd region */
    ptr_2 = lwmem_malloc(256);
    TEST_ASSERT(ptr_2 != NULL);
    IS_ALLOC_IN_REGION(ptr_2, &lw_regions[2]);

    /* Allocation of 128 bytes can be in second or third region (depends on memory availability),
        but in case of these tests it can be (and should be) in second region */
    ptr_3 = lwmem_malloc(128);
    TEST_ASSERT(ptr_3 != NULL);
    IS_ALLOC_IN_REGION(ptr_3, &lw_regions[1]);

    /* Free all pointers to default state */
    lwmem_free(ptr_1);
    lwmem_free(ptr_2);
    lwmem_free(ptr_3);

    /* Force allocation region to be used */
    /* Allocation of 16-bytes forced to 2nd region */
    ptr_1 = lwmem_malloc_ex(NULL, &lw_regions[1], 16);
    TEST_ASSERT(ptr_1 != NULL);
    IS_ALLOC_IN_REGION(ptr_1, &lw_regions[1]);

    /* Allocate ptr 2 in any region of default lwmem, the first available must be 1st region */
    ptr_2 = lwmem_malloc_ex(NULL, NULL, 16);
    TEST_ASSERT(ptr_2 != NULL);
    IS_ALLOC_IN_REGION(ptr_2, &lw_regions[0]);

    /* Free pointers */
    lwmem_free(ptr_1);
    lwmem_free(ptr_2);

    /********************************************/
    /* Run tests on custom region               */
    /********************************************/

    /* Initialize another, custom instance */
    lwmem_assignmem_ex(&lw_c, lw_c_regions);

    /* Allocation of 64 bytes must in in first region */
    ptr_c_1 = lwmem_malloc_ex(&lw_c, NULL, 64);
    TEST_ASSERT(ptr_c_1 != NULL);
    IS_ALLOC_IN_REGION(ptr_c_1, &lw_c_regions[0]);

    /* Allocation of 256 bytes can only be in 3rd region */
    ptr_c_2 = lwmem_malloc_ex(&lw_c, NULL, 256);
    TEST_ASSERT(ptr_c_2 != NULL);
    IS_ALLOC_IN_REGION(ptr_c_2, &lw_c_regions[2]);

    /* Allocation of 128 bytes can be in second or third region (depends on memory availability),
        but in case of these tests it can be (and should be) in second region */
    ptr_c_3 = lwmem_malloc_ex(&lw_c, NULL, 128);
    TEST_ASSERT(ptr_c_3 != NULL);
    IS_ALLOC_IN_REGION(ptr_c_3, &lw_c_regions[1]);

    /* Free all pointers to default state */
    lwmem_free(ptr_c_1);
    lwmem_free(ptr_c_2);
    lwmem_free(ptr_c_3);

    printf("Done\r\n");

    return 0;
}

#endif /* LWMEM_CFG_FULL */