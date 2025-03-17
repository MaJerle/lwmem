#include <iostream>
#include <stdint.h>
#include <string.h>
#include "lwmem/lwmem.h"
#include "lwmem/lwmem.hpp"

extern "C" int lwmem_test_run(void);
extern "C" int lwmem_test_simple_run(void);
extern "C" int lwmem_test_memory_structure(void);
extern "C" int lwmem_test_region(void);
extern "C" int lwmem_test_available_mem(void);

/* Setup manager */
static Lwmem::LwmemLight<1024> manager;

int
main(void) {
    int ret = 0;

    printf("----\r\n");
    ret |= lwmem_test_available_mem();
    printf("----\r\n");
    ret |= lwmem_test_region();
    printf("----\r\n");
#if LWMEM_CFG_FULL
    ret |= lwmem_test_memory_structure();
    printf("----\r\n");
#else
    ret |= lwmem_test_simple_run();
    printf("----\r\n");
#endif

    printf("ret: %u\r\n", (unsigned)ret);
    printf("Done\r\n");
    return ret;
}
