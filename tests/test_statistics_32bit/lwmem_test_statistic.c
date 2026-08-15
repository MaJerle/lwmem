#include <stdio.h>
#include "lwmem/lwmem.h"
#include "test.h"

/* Region memory declaration */
static struct {
    uint8_t m1[128];
} lw_mem;

/* Regions descriptor */
static lwmem_region_t lw_regions[] = {
    {lw_mem.m1, sizeof(lw_mem.m1)},
    {NULL, 0},
};

static void
prv_print_stats(void) {
    lwmem_stats_t stats;
    lwmem_get_stats(&stats);
    printf("Memory statistics:\n");
    printf("Mem size bytes: %zu bytes\n", stats.mem_size_bytes);
    printf("Mem available bytes: %zu bytes\n", stats.mem_available_bytes);
    printf("Mem minimum ever available bytes: %zu bytes\n", stats.minimum_ever_mem_available_bytes);
}

int
test_run(void) {
    lwmem_stats_t stats;
    void* ptr;

    /* Setup the regions */
    lwmem_assignmem(lw_regions);

    /* 
     * Starting point goes here 
     * 
     * Test runs on 32-bit systems, where sizeof(void*) == 4 bytes
     */
    lwmem_get_stats(&stats);
    TEST_ASSERT(stats.mem_size_bytes == 120);
    TEST_ASSERT(stats.mem_available_bytes == 120);
    TEST_ASSERT(stats.minimum_ever_mem_available_bytes == 120);

    /* Allocate memory and continue */
    ptr = lwmem_malloc(64);
    lwmem_get_stats(&stats);
    TEST_ASSERT(stats.mem_size_bytes == 120);
    TEST_ASSERT(stats.mem_available_bytes == 48);
    TEST_ASSERT(stats.minimum_ever_mem_available_bytes == 48);

    /* Realloc and extend by 8 bytes */
    ptr = lwmem_realloc(ptr, 72);
    lwmem_get_stats(&stats);
    TEST_ASSERT(stats.mem_size_bytes == 120);
    TEST_ASSERT(stats.mem_available_bytes == 40);
    TEST_ASSERT(stats.minimum_ever_mem_available_bytes == 40);

    /* Free memory */
    lwmem_free_s(&ptr);
    lwmem_get_stats(&stats);
    TEST_ASSERT(stats.mem_size_bytes == 120);
    TEST_ASSERT(stats.mem_available_bytes == 120);
    TEST_ASSERT(stats.minimum_ever_mem_available_bytes == 40);

    return 0;
}
