#include "lwmem/lwmem.h"
#include "lwmem/lwmem.hpp"
#include <string.h>
#include <stdint.h>
#include <iostream>

extern "C" void lwmem_test_run(void);
extern "C" void lwmem_test_memory_structure(void);

/* Setup manager */
Lwmem::LwmemLight<1024> manager;

int
main(void) {
    lwmem_test_memory_structure();
    //lwmem_test_run();

    /* Test C++ code */
    void* ret = manager.malloc(123);
    std::cout << ret << std::endl;
    manager.free(ret);

    return 0;
}
