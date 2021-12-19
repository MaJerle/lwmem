/*
 * This example is showing default LwMEM application.
 *
 * It uses simple region config, single region, without support for operating system
 */

#include "lwmem/lwmem.h"
#include <stdio.h>

/* Define single region */
uint8_t region_data[1024];
lwmem_region_t regions[] = {
    { .start_addr = region_data, .size = sizeof(region_data) },
    { .start_addr = NULL, .size = 0 }
};

int
main(void) {
    void* ptr, *ptr2;

    /* Initialize default LwMEM instance with single region */
    if (!lwmem_assignmem(regions)) {
        printf("Could not initialize LwMEM!");
        return -1;
    }
    printf("LwMEM initialized and ready to use!\r\n");

    /* Allocate memory */
    ptr = lwmem_malloc(24);
    if (ptr == NULL) {
        printf("Could not allocate memory!\r\n");
        return -1;
    }
    printf("Memory allocated!\r\n");

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
