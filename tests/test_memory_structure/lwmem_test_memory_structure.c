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

/* For debug purposes */
static lwmem_region_t* regions_used;
static size_t regions_count = 4; /* Use only 1 region for debug purposes of non-free areas */

int
test_run(void) {
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
        return -1;
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
    TEST_ASSERT(rptr1 == ptr2);

    /* Create 3b case */
    printf("\r\n------------------------------------------------------------------------\r\n");
    lwmem_debug_restore_to_saved();
    printf("State 3b\r\n");
    rptr2 = lwmem_realloc(ptr2, 20);
    lwmem_debug_print(1, 1);
    TEST_ASSERT(rptr2 == ptr1);

    /* Create 3c case */
    printf("\r\n------------------------------------------------------------------------\r\n");
    lwmem_debug_restore_to_saved();
    printf("State 3c\r\n");
    rptr3 = lwmem_realloc(ptr2, 24);
    lwmem_debug_print(1, 1);
    TEST_ASSERT(rptr3 == ptr1);

    /* Create 3d case */
    printf("\r\n------------------------------------------------------------------------\r\n");
    lwmem_debug_restore_to_saved();
    printf("State 3d\r\n");
    rptr4 = lwmem_realloc(ptr2, 36);
    lwmem_debug_print(1, 1);
    TEST_ASSERT(rptr4 != ptr1 && rptr4 != ptr2 && rptr4 != ptr3 && rptr4 != ptr4);

    printf("ptr1: %08X\r\nptr2: %08X\r\nptr3: %08X\r\nptr4: %08X\r\n", (unsigned)(uintptr_t)ptr1,
           (unsigned)(uintptr_t)ptr2, (unsigned)(uintptr_t)ptr3, (unsigned)(uintptr_t)ptr4);
    printf("r_ptr1: %08X\r\nr_ptr2: %08X\r\nr_ptr3: %08X\r\nr_ptr4: %08X\r\n", (unsigned)(uintptr_t)rptr1,
           (unsigned)(uintptr_t)rptr2, (unsigned)(uintptr_t)rptr3, (unsigned)(uintptr_t)rptr4);
    return 0;
}

#endif /* LWMEM_CFG_FULL */