#include <stdio.h>
#include "lwmem/lwmem.h"

#if LWMEM_CFG_ALIGN_NUM != 4
#error "Test shall run with LWMEM_CFG_ALIGN_NUM == 4"
#endif

/* Test assert information */
#define TEST_ASSERT(condition)                                                                                         \
    do {                                                                                                               \
        if (!(condition)) {                                                                                            \
            printf("Assert failed on the line %d\r\n", __LINE__);                                                      \
            return;                                                                                                    \
        }                                                                                                              \
    } while (0)

typedef struct {
    uint8_t* region_start;
    size_t region_size;
    uint8_t* region_start_exp;
    size_t region_size_exp;
} test_region_t;

#define TEST_ENTRY(_region_start_, _region_size_, _region_start_exp_, _region_size_exp_)                               \
    {                                                                                                                  \
        .region_start = (void*)(_region_start_),                                                                       \
        .region_size = (_region_size_),                                                                                \
        .region_start_exp = (void*)(_region_start_exp_),                                                               \
        .region_size_exp = (_region_size_exp_),                                                                        \
    }

/* List of test cases */
static const test_region_t test_cases[] = {
    TEST_ENTRY(0x00000000, 0x00000000, 0x00000000, 0x00000000),
    TEST_ENTRY(0x00000000, 0x00004000, 0x00000000, 0x00004000),
    TEST_ENTRY(0x00000000, 0x00004001, 0x00000000, 0x00004000),
    TEST_ENTRY(0x00000000, 0x00004002, 0x00000000, 0x00004000),
    TEST_ENTRY(0x00000000, 0x00004003, 0x00000000, 0x00004000),
    TEST_ENTRY(0x00000000, 0x00004004, 0x00000000, 0x00004004),
    TEST_ENTRY(0x00000000, 0x00004005, 0x00000000, 0x00004004),

    /* Start is not always aligned, but length input is aligned */
    TEST_ENTRY(0x00000000, 0x00004000, 0x00000000, 0x00004000),

    /* Start must advance to next aligned value, size reduced to next lower aligned value */
    TEST_ENTRY(0x00000001, 0x00004000, 0x00000004, 0x00003FFC),
    TEST_ENTRY(0x00000002, 0x00004000, 0x00000004, 0x00003FFC),
    TEST_ENTRY(0x00000003, 0x00004000, 0x00000004, 0x00003FFC),
    TEST_ENTRY(0x00000003, 0x00004003, 0x00000004, 0x00004000),
    TEST_ENTRY(0x00000003, 0x00004004, 0x00000004, 0x00004000),
    TEST_ENTRY(0x00000003, 0x00004005, 0x00000004, 0x00004004),
    TEST_ENTRY(0x00000001, 0x00004003, 0x00000004, 0x00004000),
    TEST_ENTRY(0x00000002, 0x00004005, 0x00000004, 0x00004000),
    TEST_ENTRY(0x00000002, 0x00004006, 0x00000004, 0x00004004),
    TEST_ENTRY(0x00000002, 0x00004007, 0x00000004, 0x00004004),
    TEST_ENTRY(0x00000003, 0x00004006, 0x00000004, 0x00004004),
    TEST_ENTRY(0x00000003, 0x00004005, 0x00000004, 0x00004004),
    TEST_ENTRY(0x00000004, 0x00004006, 0x00000004, 0x00004004),
};

/* Start is aligned, length is not always aligned */

void
lwmem_test_region(void) {
    uint8_t* region_start = NULL;
    size_t region_size = 0;

    for (size_t idx = 0; idx < sizeof(test_cases) / sizeof(test_cases[0]); ++idx) {
        const test_region_t* test = &test_cases[idx];
        lwmem_debug_test_region(test->region_start, test->region_size, &region_start, &region_size);

        if (region_start != test->region_start_exp) {
            printf("Region start test failed. Idx: %u, input: 0x%08p, expected: 0x%08p, output: 0x%08p\r\n",
                   (unsigned)idx, test->region_start, test->region_start_exp, region_start);
        } else if (region_size != test->region_size_exp) {
            printf("Region size test failed. Idx: %u, input: 0x%08X, expected: 0x%08X, output: 0x%08X\r\n",
                   (unsigned)idx, (unsigned)test->region_size, (unsigned)test->region_size_exp, (unsigned)region_size);
        }
    }

    printf("Done\r\n");
}
