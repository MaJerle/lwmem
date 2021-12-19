/*
 * This example is showing LwMEM in multiple regions for default instance
 *
 * It configures 2 regions and shows simple allocation features
 */

#include "lwmem/lwmem.h"
#include <stdio.h>
#include <string.h>

/* Define multiple regions */
uint8_t region1_data[1024];
uint8_t region2_data[256];
lwmem_region_t regions[] = {
    { .start_addr = region1_data, .size = sizeof(region1_data) },
    { .start_addr = region2_data, .size = sizeof(region2_data) },
    { .start_addr = NULL, .size = 0 },
};

int
main(void) {
    void* ptr, *ptr2;

    /* Check if order of addresses is lower first */
    if (regions[1].start_addr < regions[0].start_addr) {
        lwmem_region_t r;
        memcpy(&r, &regions[1], sizeof(r));
        memcpy(&regions[1], &regions[0], sizeof(r));
        memcpy(&regions[0], &r, sizeof(r));
    }

    /* Initialize default LwMEM instance with single region */
    if (!lwmem_assignmem(regions)) {
        printf("Could not initialize LwMEM!");
        return -1;
    }
    printf("LwMEM initialized and ready to use!\r\n");

    /* Force memory allocation in specific region */
    ptr = lwmem_malloc_ex(NULL, &regions[1], 24);
    if (ptr == NULL) {
        printf("Could not allocate memory in second region!\r\n");
        return -1;
    }
    printf("Memory allocated from second region!\r\n");

    /* Increase its size */
    ptr2 = lwmem_realloc(ptr, 48);
    if (ptr2 == NULL) {
        printf("Could not reallocate existing ptr\r\n");
    } else {
        printf("Memory reallocated!\r\n");
        ptr = ptr2;
        ptr2 = NULL;
    }

    /* Free memory after */
    lwmem_free(ptr);
}
