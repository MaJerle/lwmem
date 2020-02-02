#include <stdio.h>
#include "lwmem/lwmem.h"
#include "string.h"
#include "stdint.h"

extern void lwmem_test_run(void);
extern void lwmem_test_memory_structure(void);

int
main(void) {
    lwmem_test_memory_structure();
    //lwmem_test_run();
    return 0;
}
