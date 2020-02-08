#include "lwmem/lwmem.h"

/* Assignment has been done previously... */

/* ptr1 will be allocated in first free block */
/* ptr2 will be allocated from second region */
void* ptr1, *ptr2;

/* Allocate 8 bytes of memory in any region */
/* Use one of 2 options, both have same effect */
ptr1 = lwmem_malloc(8);
ptr1 = lwmem_malloc_ex(NULL, NULL, 8);

/* Allocate memory from specific region only */
/* Use second region */
ptr2 = lwmem_malloc_ex(NULL, &regions[1], 512);