#include <stdio.h>
#include <string.h>
#include "lwmem/lwmem.h"
#include "test.h"

#if defined(_WIN32)
#include <windows.h>
#else
#include <pthread.h>
#endif

#define OS_TEST_THREADS    4
#define OS_TEST_ITERATIONS 500

/* Region memory shared by all threads, protected by the LWMEM_CFG_OS mutex */
static struct {
    uint8_t m1[4096];
} lw_mem;

static lwmem_region_t lw_regions[] = {
    {lw_mem.m1, sizeof(lw_mem.m1)},
    {NULL, 0},
};

/* Set to nonzero by a worker if it detects corrupted/overlapping data */
static volatile int corruption_detected = 0;

/* Small deterministic per-thread PRNG (rand() is not required to be thread-safe) */
static uint32_t
prv_next_rand(uint32_t* state) {
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

static void
prv_worker(int thread_idx) {
    uint32_t rand_state = 0x9E3779B9u ^ (uint32_t)(thread_idx + 1);
    uint8_t pattern = (uint8_t)(0xA0 + thread_idx);

    for (int i = 0; i < OS_TEST_ITERATIONS; ++i) {
        size_t size = 8 + (prv_next_rand(&rand_state) % 57); /* 8..64 bytes */
        uint8_t* ptr = lwmem_malloc(size);
        if (ptr == NULL) {
            /* Region can legitimately run out of memory under contention; skip this round */
            continue;
        }

        /* Stamp the block with a pattern unique to this thread and verify
         * nobody else's allocation overlaps it while we hold it */
        memset(ptr, pattern, size);
        for (size_t j = 0; j < size; ++j) {
            if (ptr[j] != pattern) {
                corruption_detected = 1;
                break;
            }
        }

        lwmem_free(ptr);
    }
}

#if defined(_WIN32)
static DWORD WINAPI
prv_thread_func(LPVOID arg) {
    prv_worker((int)(intptr_t)arg);
    return 0;
}
#else
static void*
prv_thread_func(void* arg) {
    prv_worker((int)(intptr_t)arg);
    return NULL;
}
#endif

int
test_run(void) {
    lwmem_stats_t stats_before, stats_after;

    /* Setup the memory region */
    TEST_ASSERT(lwmem_assignmem(lw_regions) > 0);
    lwmem_get_stats(&stats_before);

#if defined(_WIN32)
    HANDLE threads[OS_TEST_THREADS];
    for (int i = 0; i < OS_TEST_THREADS; ++i) {
        threads[i] = CreateThread(NULL, 0, prv_thread_func, (LPVOID)(intptr_t)i, 0, NULL);
        TEST_ASSERT(threads[i] != NULL);
    }
    WaitForMultipleObjects(OS_TEST_THREADS, threads, TRUE, INFINITE);
    for (int i = 0; i < OS_TEST_THREADS; ++i) {
        CloseHandle(threads[i]);
    }
#else
    pthread_t threads[OS_TEST_THREADS];
    for (int i = 0; i < OS_TEST_THREADS; ++i) {
        TEST_ASSERT(pthread_create(&threads[i], NULL, prv_thread_func, (void*)(intptr_t)i) == 0);
    }
    for (int i = 0; i < OS_TEST_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }
#endif

    /* No corrupted/overlapping allocation was ever observed under concurrent access */
    TEST_ASSERT(!corruption_detected);

    /* Every allocation was freed again -- available bytes must be back to the starting point */
    lwmem_get_stats(&stats_after);
    TEST_ASSERT(stats_after.mem_available_bytes == stats_before.mem_available_bytes);

    printf("OS thread-safety test done\r\n");

    return 0;
}
