/*
 * This example is showing default LwMEM configuration for operating systems.
 *
 * It uses simple region config and has mutex feature enabled (see lwmem_config.h file).
 * Multiple threads try to access to same resource at the same time
 */

#include "lwmem/lwmem.h"
#include <stdio.h>

/* Define single region */
uint8_t region_data[1024];
lwmem_region_t regions[] = {
    { .start_addr = region_data, .size = sizeof(region_data) },
    { .start_addr = NULL, .size = 0 },
};

/* Thread declaration */
static int thread_func(void* arg);

int
main(void) {
    /* Initialize default LwMEM instance with single region */
    if (!lwmem_assignmem(regions)) {
        printf("Could not initialize LwMEM!");
        return -1;
    }
    printf("LwMEM initialized and ready to use!\r\n");

    /* Create multiple threads */
    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread_func, NULL, 0, NULL);
    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread_func, NULL, 0, NULL);
    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread_func, NULL, 0, NULL);

    /* Sleep to let threads to finish */
    Sleep(1000);
    return 0;
}

/**
 * \brief           Thread function, multiple instances of same thread are executed
 *                  in paralled and created with CreateThread function from OS
 */
static int
thread_func(void* arg) {
    void* ptr, * ptr2;

    (void)arg;

    /* Allocate memory */
    if ((ptr = lwmem_malloc(24)) == NULL) {
        printf("Could not allocate memory!\r\n");
        return -1;
    }
    printf("Memory allocated at address 0x%p!\r\n", ptr);

    /* Increase its size */
    if ((ptr2 = lwmem_realloc(ptr, 48)) == NULL) {
        printf("Could not reallocate existing ptr\r\n");
    } else {
        printf("Memory reallocated at address 0x%p!\r\n", ptr2);
        ptr = ptr2;
        ptr2 = NULL;
    }

    /* Free memory after */
    lwmem_free(ptr);

    return 0;
}
