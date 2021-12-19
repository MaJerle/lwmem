#include "lwmem/lwmem.h"

/**
 * \brief           Custom LwMEM instance
 */
static
lwmem_t lw_custom;

/*
 * \brief           Define regions for memory manager
 */
static
lwmem_region_t regions[] = {
    /* Set start address and size of each region */
    { (void *)0x10000000, 0x00001000 },
    { (void *)0xA0000000, 0x00008000 },
    { (void *)0xC0000000, 0x00008000 },
    { NULL, 0 }
};

/* Later in the initialization process */
/* Assign regions for custom instance */
lwmem_assignmem_ex(&lw_custom, regions);