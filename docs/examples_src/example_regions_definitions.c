#include "lwmem/lwmem.h"

/* Set to 1 to describe regions only with array */
#define DESCRIBE_REGIONS_WITH_ARRAY_ONLY            1

/*
 * \brief           Define regions for memory manager
 */
static
lwmem_region_t regions[] = {
    /* Set start address and size of each region */
    { (void *)0x10000000, 0x00001000 },
    { (void *)0xA0000000, 0x00008000 },
    { (void *)0xC0000000, 0x00008000 },
#if DESCRIBE_REGIONS_WITH_ARRAY_ONLY
    { NULL, 0},
#endif
};

/* Later in the initialization process */
/* Assign regions for manager */
lwmem_assignmem(regions, DESCRIBE_REGIONS_WITH_ARRAY_ONLY ? 0 : (sizeof(regions) / sizeof(regions[0])));
/* or */
lwmem_assignmem_ex(NULL, regions, DESCRIBE_REGIONS_WITH_ARRAY_ONLY ? 0 : (sizeof(regions) / sizeof(regions[0])));