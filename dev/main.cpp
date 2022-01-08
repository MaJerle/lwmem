#include <stdio.h>
#include "lwmem/lwmem.h"
#include <string.h>
#include <stdint.h>

extern "C" void lwmem_test_run(void);
extern "C" void lwmem_test_memory_structure(void);

namespace Lwmem {

class Lwmem {
public:
    Lwmem(const lwmem_region_t* regions = nullptr) {
        if (regions != nullptr
            && lwmem_assignmem_ex(&m_lw, m_regions)) {
            m_regions = regions;
        }
    }

    uint8_t
    set_regions(const lwmem_region_t* regions) {
        if (m_regions == nullptr && regions != nullptr
            && lwmem_assignmem_ex(&m_lw, m_regions)) {
            m_regions = regions;
            return 1;
        }
        return 0;
    }

    void*
    malloc_ex(const lwmem_region_t* region, size_t size) {
        return lwmem_malloc_ex(&m_lw, region, size);
    }
    void*
    malloc(size_t size) {
        return malloc_ex(NULL, size);
    }
    void*
    calloc_ex(const lwmem_region_t* region, size_t nitems, size_t size) {
        return lwmem_calloc_ex(&m_lw, region, nitems, size);
    }
    void*
    calloc(size_t nitems, size_t size) {
        return calloc_ex(NULL, nitems, size);
    }
    void*
    realloc_ex(const lwmem_region_t* region, void* ptr, size_t size) {
        return lwmem_realloc_ex(&m_lw, region, ptr, size);
    }
    void*
    realloc(void* ptr, size_t size) {
        return realloc_ex(NULL, ptr, size);
    }

    void
    free(void* ptr) {
        lwmem_free_ex(&m_lw, ptr);
    }

private:
    /* Delete unused constructors */
    Lwmem() = delete;
    Lwmem(const Lwmem& other) = delete;
    /* Delete copy assignment operators */
    Lwmem& operator=(const Lwmem& other) = delete;
    Lwmem* operator=(const Lwmem* other) = delete;

    lwmem_t m_lw;
    const lwmem_region_t* m_regions = nullptr;
};

};

int
main(void) {
    lwmem_test_memory_structure();
    //lwmem_test_run();

    Lwmem::Lwmem manager(nullptr);
    void* ret = manager.malloc(123);

    /* blablablabla */

    manager.free(ret);

    return 0;
}
