/*
 * This example is adding custom LwMEM instance of top of default one.
 *
 * Effectively 2 instances are active, each with its own set of regions.
 * Purpose is to show the difference between _ex and non _ex functions.
 */

#include "lwmem/lwmem.h"
#include <stdio.h>
#include <string.h>

/* Define multiple regions for default instance */
uint8_t lw0_region1_data[1024];
uint8_t lw0_region2_data[256];
lwmem_region_t lw0_regions[] = {
    { .start_addr = lw0_region1_data, .size = sizeof(lw0_region1_data) },
    { .start_addr = lw0_region2_data, .size = sizeof(lw0_region2_data) },
    { .start_addr = NULL, .size = 0 },
};

/* Define second LwMEM instance and multiple regions for new instance */
lwmem_t lw1;
uint8_t lw1_region1_data[1024];
uint8_t lw1_region2_data[512];
lwmem_region_t lw1_regions[] = {
    { .start_addr = lw1_region1_data, .size = sizeof(lw1_region1_data) },
    { .start_addr = lw1_region2_data, .size = sizeof(lw1_region2_data) },
    { .start_addr = NULL, .size = 0 },
};

int
main(void) {
    void* lw0_ptr1, *lw0_ptr2, *lw0_ptr3;
    void* lw1_ptr1, *lw1_ptr2;

    /* Check if order of addresses is lower first */
    if (lw0_regions[1].start_addr < lw0_regions[0].start_addr) {
        lwmem_region_t r;
        memcpy(&r, &lw0_regions[1], sizeof(r));
        memcpy(&lw0_regions[1], &lw0_regions[0], sizeof(r));
        memcpy(&lw0_regions[0], &r, sizeof(r));
    }
    if (lw1_regions[1].start_addr < lw1_regions[0].start_addr) {
        lwmem_region_t r;
        memcpy(&r, &lw1_regions[1], sizeof(r));
        memcpy(&lw1_regions[1], &lw1_regions[0], sizeof(r));
        memcpy(&lw1_regions[0], &r, sizeof(r));
    }

    /* Initialize default LwMEM instance with single region */
    if (!lwmem_assignmem(lw0_regions)) {
        printf("Could not initialize default LwMEM instance!");
        return -1;
    }
    printf("Default LwMEM instance initialized and ready to use!\r\n");

    /* Initialize custom LwMEM instance with its custom regions */
    if (!lwmem_assignmem_ex(&lw1, lw1_regions)) {
        printf("Could not initialize custom LwMEM instance!");
        return -1;
    }
    printf("Custom LwMEM instance initialized and ready to use!\r\n");

    /* Memory operations for default instance */
    lw0_ptr1 = lwmem_malloc(24);        /* Allocate memory from default LwMEM instance, any region */
    lw0_ptr2 = lwmem_malloc_ex(NULL, NULL, 24); /* Allocate memory from default LwMEM instance, any region */
    lw0_ptr3 = lwmem_malloc_ex(NULL, &lw0_regions[1], 24);  /* Allocate memory from default LwMEM instance, force second region */

    /* Free memory after use from default region */
    lwmem_free(lw0_ptr1);
    lwmem_free_ex(NULL, lw0_ptr2);
    lwmem_free_ex(NULL, lw0_ptr3);

    /* Memory operations for custom instance */
    lw1_ptr1 = lwmem_malloc_ex(&lw1, NULL, 24); /* Allocate memory from custom LwMEM instance, any region */
    lw1_ptr2 = lwmem_malloc_ex(&lw1, &lw1_regions[1], 24);  /* Allocate memory from default LwMEM instance, force second region */

    (void)lw1_ptr1;
    (void)lw1_ptr2;

    /* Free memory after use */
    lwmem_free_ex(&lw1, lw0_ptr1);
    lwmem_free_ex(&lw1, lw0_ptr2);
}
