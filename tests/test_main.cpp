#include <iostream>
#include <stdint.h>
#include <string.h>
#include "lwmem/lwmem.h"
#include "lwmem/lwmem.hpp"

extern "C" int test_run(void);

int
main(void) {
    int ret = 0;
    printf("Application running\r\n");
    ret = test_run();
    printf("Test finished\r\n");
    return ret;
}
