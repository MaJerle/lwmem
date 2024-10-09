#include <stdio.h>
#include "lwmem/lwmem.h"

#if LWMEM_CFG_FULL

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
/* Test case helpers                        */
#define UINT_PTR_CAST(x) ((uintptr_t)(x))
#define IS_ALLOC_IN_REGION(ptr, region)                                                                                \
    ASSERT(UINT_PTR_CAST(ptr) >= UINT_PTR_CAST((region)->start_addr)                                                   \
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

void
lwmem_test_run(void) {
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
    ASSERT(ptr_1 != NULL);
    IS_ALLOC_IN_REGION(ptr_1, &lw_regions[0]);

    /* Allocation of 256 bytes can only be in 3rd region */
    ptr_2 = lwmem_malloc(256);
    ASSERT(ptr_2 != NULL);
    IS_ALLOC_IN_REGION(ptr_2, &lw_regions[2]);

    /* Allocation of 128 bytes can be in second or third region (depends on memory availability),
        but in case of these tests it can be (and should be) in second region */
    ptr_3 = lwmem_malloc(128);
    ASSERT(ptr_3 != NULL);
    IS_ALLOC_IN_REGION(ptr_3, &lw_regions[1]);

    /* Free all pointers to default state */
    lwmem_free(ptr_1);
    lwmem_free(ptr_2);
    lwmem_free(ptr_3);

    /* Force allocation region to be used */
    /* Allocation of 16-bytes forced to 2nd region */
    ptr_1 = lwmem_malloc_ex(NULL, &lw_regions[1], 16);
    ASSERT(ptr_1 != NULL);
    IS_ALLOC_IN_REGION(ptr_1, &lw_regions[1]);

    /* Allocate ptr 2 in any region of default lwmem, the first available must be 1st region */
    ptr_2 = lwmem_malloc_ex(NULL, NULL, 16);
    ASSERT(ptr_2 != NULL);
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
    ASSERT(ptr_c_1 != NULL);
    IS_ALLOC_IN_REGION(ptr_c_1, &lw_c_regions[0]);

    /* Allocation of 256 bytes can only be in 3rd region */
    ptr_c_2 = lwmem_malloc_ex(&lw_c, NULL, 256);
    ASSERT(ptr_c_2 != NULL);
    IS_ALLOC_IN_REGION(ptr_c_2, &lw_c_regions[2]);

    /* Allocation of 128 bytes can be in second or third region (depends on memory availability),
        but in case of these tests it can be (and should be) in second region */
    ptr_c_3 = lwmem_malloc_ex(&lw_c, NULL, 128);
    ASSERT(ptr_c_3 != NULL);
    IS_ALLOC_IN_REGION(ptr_c_3, &lw_c_regions[1]);

    /* Free all pointers to default state */
    lwmem_free(ptr_c_1);
    lwmem_free(ptr_c_2);
    lwmem_free(ptr_c_3);

    printf("Done\r\n");
}

/* For debug purposes */
static lwmem_region_t* regions_used;
static size_t regions_count = 4; /* Use only 1 region for debug purposes of non-free areas */

void
lwmem_test_memory_structure(void) {
    uint8_t *ptr1, *ptr2, *ptr3, *ptr4;
    uint8_t *rptr1, *rptr2, *rptr3, *rptr4;
    size_t used_regions;

    /*
     * Create regions for debug purpose
     *
     * Size of regions array is 1 longer than count passed,
     * and has size set to 0 and address to NULL
     */
    if (!lwmem_debug_create_regions(&regions_used, regions_count, 128)) {
        printf("Cannot allocate memory for regions for debug purpose!\r\n");
        return;
    }

    /*
     * Assign memory for LwMEM. Set len parameter to 0 to calculate
     * Number of regions with regions pointer, with last entry being set to NULL and 0
     */
    used_regions = lwmem_assignmem(regions_used);
    printf("Manager is ready with %d regions!\r\n", (int)used_regions);
    lwmem_debug_print(1, 1);

    /* Test case 1, allocate 3 blocks, each of different size */
    /* We know that sizeof internal metadata block is 8 bytes on win32 */
    printf("\r\n------------------------------------------------------------------------\r\n");
    printf("Allocating 4 pointers\r\n\r\n");
    ptr1 = lwmem_malloc(8);
    ptr2 = lwmem_malloc(4);
    ptr3 = lwmem_malloc(4);
    ptr4 = lwmem_malloc(16);
    lwmem_debug_print(1, 1);
    printf("\r\n------------------------------------------------------------------------\r\n");
    printf("Freeing first and third pointers\r\n\r\n");
    lwmem_free(ptr1); /* Free but keep value for future comparison */
    lwmem_free(ptr3); /* Free but keep value for future comparison */
    lwmem_debug_print(1, 1);
    printf("Debug above is effectively state 3\r\n");
    lwmem_debug_save_state(); /* Every restore operations rewinds here */

    /* We always try to reallocate pointer ptr2 */

    /* Create 3a case */
    printf("\r\n------------------------------------------------------------------------\r\n");
    lwmem_debug_restore_to_saved();
    printf("State 3a\r\n");
    rptr1 = lwmem_realloc(ptr2, 8);
    lwmem_debug_print(1, 1);
    ASSERT(rptr1 == ptr2);

    /* Create 3b case */
    printf("\r\n------------------------------------------------------------------------\r\n");
    lwmem_debug_restore_to_saved();
    printf("State 3b\r\n");
    rptr2 = lwmem_realloc(ptr2, 20);
    lwmem_debug_print(1, 1);
    ASSERT(rptr2 == ptr1);

    /* Create 3c case */
    printf("\r\n------------------------------------------------------------------------\r\n");
    lwmem_debug_restore_to_saved();
    printf("State 3c\r\n");
    rptr3 = lwmem_realloc(ptr2, 24);
    lwmem_debug_print(1, 1);
    ASSERT(rptr3 == ptr1);

    /* Create 3d case */
    printf("\r\n------------------------------------------------------------------------\r\n");
    lwmem_debug_restore_to_saved();
    printf("State 3d\r\n");
    rptr4 = lwmem_realloc(ptr2, 36);
    lwmem_debug_print(1, 1);
    ASSERT(rptr4 != ptr1 && rptr4 != ptr2 && rptr4 != ptr3 && rptr4 != ptr4);

    printf("ptr1: %08X\r\nptr2: %08X\r\nptr3: %08X\r\nptr4: %08X\r\n", (unsigned)ptr1, (unsigned)ptr2, (unsigned)ptr3,
           (unsigned)ptr4);
    printf("r_ptr1: %08X\r\nr_ptr2: %08X\r\nr_ptr3: %08X\r\nr_ptr4: %08X\r\n", (unsigned)rptr1, (unsigned)rptr2,
           (unsigned)rptr3, (unsigned)rptr4);
}

#endif /* LWMEM_CFG_FULL */