#include "lwmem/lwmem.h"

/* Create regions, address and length of regions */
static
lwmem_region_t regions[] = {
    /* Set start address and size of each region */
    { (void *)0x10000000, 0x00001000 },
    { (void *)0xA0000000, 0x00008000 },
    { (void *)0xC0000000, 0x00008000 },
    { NULL, 0}
};

/* Later in the initialization process */
/* Assign regions for manager */
lwmem_assignmem(regions);

/* Usage in program... */

void* ptr;
/* Allocate 8 bytes of memory */
ptr = lwmem_malloc(8);
if (ptr != NULL) {
    /* Allocation successful */
}

/* Later... */                                  
/* Free allocated memory when not used */
lwmem_free(ptr);
ptr = NULL;
/* .. or */
lwmem_free_s(&ptr);