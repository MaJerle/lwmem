#include <iostream>
#include <stdint.h>
#include <string.h>
#include "lwmem/lwmem.h"
#include "lwmem/lwmem.hpp"

extern "C" void lwmem_test_run(void);
extern "C" void lwmem_test_simple_run(void);
extern "C" void lwmem_test_memory_structure(void);
extern "C" void lwmem_test_region(void);
extern "C" void lwmem_test_available_mem(void);

/* Setup manager */
static Lwmem::LwmemLight<1024> manager;

int
main(void) {
    lwmem_test_available_mem();
    return 0;
    lwmem_test_region();
    return 0;
#if LWMEM_CFG_FULL
    lwmem_test_memory_structure();
    //lwmem_test_run();
#else
    lwmem_test_simple_run();
#endif

#if 1
    /* Test C++ code */
    void* ret = manager.malloc(123);
    std::cout << ret << std::endl;
#if LWMEM_CFG_FULL
    manager.free(ret);
#endif /* LWMEM_CFG_FULL */
#endif

    return 0;
}
