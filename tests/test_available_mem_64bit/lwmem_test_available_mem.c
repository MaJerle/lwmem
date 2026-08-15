/**
 * \file            lwmem_test_available_mem.c
 * \author          Tilen MAJERLE <tilen@majerle.eu>
 * \brief           
 * \version         0.1
 * \date            2025-03-30
 * 
 * @copyright Copyright (c) 2025
 * 
 * This test assumes 64-bit architecture (8-bytes void*)
 */
#include <stdio.h>
#include "lwmem/lwmem.h"
#include "test.h"

#if LWMEM_CFG_ALIGN_NUM != 4
#error "Test shall run with LWMEM_CFG_ALIGN_NUM == 4"
#endif

int
test_run(void) {
    static lwmem_t lwmem;
    uint32_t data_buff[256 >> 2];
    void *par1 = NULL, *par2 = NULL, *par3 = NULL;
    const lwmem_region_t regions[] = {
        {data_buff, sizeof(data_buff)},
        {NULL, 0},
    };

    /* Setup the values */
    lwmem_assignmem_ex(&lwmem, regions);
    TEST_ASSERT(lwmem.mem_available_bytes == 240);

    /* Allocate 10 bytes, that shall be properly aligned + header used */
    par1 = lwmem_malloc_ex(&lwmem, NULL, 10);
    TEST_ASSERT(par1 != NULL);
    TEST_ASSERT(lwmem.mem_available_bytes == (212));
    par2 = lwmem_malloc_ex(&lwmem, NULL, 10);
    TEST_ASSERT(par2 != NULL);
    TEST_ASSERT(lwmem.mem_available_bytes == (184));
    par3 = lwmem_malloc_ex(&lwmem, NULL, 10);
    TEST_ASSERT(par3 != NULL);
    TEST_ASSERT(lwmem.mem_available_bytes == (156));

    /* Free the entry and check the new available bytes */
    lwmem_free_s_ex(&lwmem, &par3);
    TEST_ASSERT(par3 == NULL);
    TEST_ASSERT(lwmem.mem_available_bytes == (184));

    /* Free remaining 2 */
    lwmem_free_s_ex(&lwmem, &par2);
    TEST_ASSERT(par2 == NULL);
    TEST_ASSERT(lwmem.mem_available_bytes == (212));
    lwmem_free_s_ex(&lwmem, &par1);
    TEST_ASSERT(par1 == NULL);
    TEST_ASSERT(lwmem.mem_available_bytes == (240));

    return 0;
}
