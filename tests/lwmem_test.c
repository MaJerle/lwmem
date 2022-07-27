#include "lwmem/lwmem.h"
#include <stdio.h>

/* Assert check */
#define ASSERT(x)           do {        \
    if (!(x)) {                         \
        printf("Assert on line %d failed with condition (" # x ")\r\n", (int)__LINE__);  \
    } else {\
        printf("Assert on line %d passed with condition (" # x ")\r\n", (int)__LINE__);  \
    }\
} while (0) 

/********************************************/
/* Test case helpers                        */
#define UINT_PTR_CAST(x)                        ((uintptr_t)(x))
#define IS_ALLOC_IN_REGION(ptr, region)         ASSERT(                             \
    UINT_PTR_CAST(ptr) >= UINT_PTR_CAST((region)->start_addr)                       \
    && UINT_PTR_CAST(ptr) < (UINT_PTR_CAST((region)->start_addr) + (region)->size)  \
)

/********************************************/
/* Configuration for default lwmem instance */

/* Region memory declaration */
uint8_t lw_mem1[1024], lw_mem2[256], lw_mem3[128];

/* Regions descriptor */
lwmem_region_t
lw_regions[] = {
    { lw_mem3, sizeof(lw_mem3) },
    { lw_mem2, sizeof(lw_mem2) },
    { lw_mem1, sizeof(lw_mem1) },
    { NULL, 0 }
};

/********************************************/
/********************************************/
/* Configuration for custom lwmem instance  */
/* LwMEM instance */
lwmem_t lw_c;

/* Region memory declaration */
uint8_t lw_c_mem1[1024], lw_c_mem2[256], lw_c_mem3[128];

/* Regions descriptor */
lwmem_region_t
lw_c_regions[] = {
    { lw_c_mem3, sizeof(lw_c_mem3) },
    { lw_c_mem2, sizeof(lw_c_mem2) },
    { lw_c_mem1, sizeof(lw_c_mem1) },
    { NULL, 0 }
};
/********************************************/

void
lwmem_test_run(void) {
    void* ptr_1, * ptr_2, * ptr_3;
    void* ptr_c_1, * ptr_c_2, * ptr_c_3;

    /* Initialize default lwmem instance */
    /* Use one of 2 possible function calls: */
    lwmem_assignmem(lw_regions);
    //lwmem_assignmem_ex(NULL, lw_regions);

    /* Initialize another, custom instance */
    lwmem_assignmem_ex(&lw_c, lw_c_regions);

    /* Regions initialized... */

    /********************************************/
    /* Run tests on default region              */
    /********************************************/

    /* Allocation of 64 bytes must in in first region */
    ptr_1 = lwmem_malloc(64);
    IS_ALLOC_IN_REGION(ptr_1, &lw_regions[0]);

    /* Allocation of 256 bytes can only be in 3rd region */
    ptr_2 = lwmem_malloc(256);
    IS_ALLOC_IN_REGION(ptr_2, &lw_regions[2]);

    /* Allocation of 128 bytes can be in second or third region (depends on memory availability),
        but in case of these tests it can be (and should be) in second region */
    ptr_3 = lwmem_malloc(128);
    IS_ALLOC_IN_REGION(ptr_3, &lw_regions[1]);

    /* Free all pointers to default state */
    lwmem_free(ptr_1);
    lwmem_free(ptr_2);
    lwmem_free(ptr_3);

    /* Force allocation region to be used */
    /* Allocation of 16-bytes forced to 2nd region */
    ptr_1 = lwmem_malloc_ex(NULL, &lw_regions[1], 16);
    IS_ALLOC_IN_REGION(ptr_1, &lw_regions[1]);

    /* Allocate ptr 2 in any region of default lwmem, the first available must be 1st region */
    ptr_2 = lwmem_malloc_ex(NULL, NULL, 16);
    IS_ALLOC_IN_REGION(ptr_2, &lw_regions[0]);

    /* Free pointers */
    lwmem_free(ptr_1);
    lwmem_free(ptr_2);

    /********************************************/
    /* Run tests on custom region               */
    /********************************************/

    /* Allocation of 64 bytes must in in first region */
    ptr_c_1 = lwmem_malloc_ex(&lw_c, NULL, 64);
    IS_ALLOC_IN_REGION(ptr_c_1, &lw_c_regions[0]);

    /* Allocation of 256 bytes can only be in 3rd region */
    ptr_c_2 = lwmem_malloc_ex(&lw_c, NULL, 256);
    IS_ALLOC_IN_REGION(ptr_c_2, &lw_c_regions[2]);

    /* Allocation of 128 bytes can be in second or third region (depends on memory availability),
        but in case of these tests it can be (and should be) in second region */
    ptr_c_3 = lwmem_malloc_ex(&lw_c, NULL, 128);
    IS_ALLOC_IN_REGION(ptr_c_3, &lw_c_regions[1]);

    /* Free all pointers to default state */
    lwmem_free(ptr_c_1);
    lwmem_free(ptr_c_2);
    lwmem_free(ptr_c_3);
}

/* For debug purposes */
lwmem_region_t* regions_used;
size_t regions_count = 4;       /* Use only 1 region for debug purposes of non-free areas */

void
lwmem_test_memory_structure(void) {
    uint8_t* ptr1, *ptr2, *ptr3, *ptr4;
    uint8_t* rptr1, *rptr2, *rptr3, *rptr4;
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
    printf("\r\n\r\nAllocating 4 pointers and freeing first and third..\r\n");
    ptr1 = lwmem_malloc(8);
    ptr2 = lwmem_malloc(4);
    ptr3 = lwmem_malloc(4);
    ptr4 = lwmem_malloc(16);
    lwmem_free(ptr1);               /* Free but keep value for future comparison */
    lwmem_free(ptr3);               /* Free but keep value for future comparison */
    lwmem_debug_print(1, 1);
    printf("Debug above is effectively state 3\r\n");
    lwmem_debug_save_state();       /* Every restore operations rewinds here */

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
    ASSERT(rptr2 == ptr2);

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
}