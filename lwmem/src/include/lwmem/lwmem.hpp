/**
 * \file            lwmem.hpp
 * \brief           Lightweight dynamic memory manager - C++ wrapper
 */

/*
 * Copyright (c) 2020 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwMEM - Lightweight dynamic memory manager library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v2.2.4
 */
#ifndef LWMEM_HDR_HPP
#define LWMEM_HDR_HPP

#include "lwmem/lwmem.h"

/**
 * \ingroup         LWMEM
 * \defgroup        LWMEM_CPP C++ wrapper functions for LwMEM
 * \brief           C++ wrapper functions for LwMEM
 * \{
 */

namespace Lwmem {

/**
 * \brief           LwMEM Light implementation with single memory region.
 * \tparam          LEN: Length of region in units of bytes 
 * 
 * This class provides C++ wrapper functions for LwMEM library.
 * For detailed docs instructions, have a look at \ref lwmem.c file.
 * 
 * Start lwmem with:
 * \code{.c}
Lwmem::LwmemLight<1024> mngr;   //Use 1024 bytes of data for memory operations
void* ptr = mngr.malloc(...);
...
...
mngr.free(ptr);
\endcode
 */
template <size_t LEN>
class LwmemLight {
  public:
    LwmemLight() {
        /* Simple region descriptor with one region */
        const lwmem_region_t regions[] = {{m_reg_data, sizeof(m_reg_data)}, {NULL, 0}};
        lwmem_assignmem_ex(&m_lw, regions);
    }

    /**
     * \brief           Allocate block of memory with selected size
     * \param           size: Block size to allocate in units of bytes
     * \return          Allocated memory or `NULL` on failure
     * \sa              lwmem_malloc_ex
     */
    void*
    malloc(size_t size) {
        return lwmem_malloc_ex(&m_lw, nullptr, size);
    }

    /**
     * \brief           Allocate block of memory with selected size and cleaned to all zeros
     * \param[in]       nitems: Number of items to allocate
     * \param           size: Size of each item in units of bytes
     * \return          Allocated memory or `NULL` on failure
     * \sa              lwmem_calloc_ex
     */
    void*
    calloc(size_t nitems, size_t size) {
        return lwmem_calloc_ex(&m_lw, nullptr, nitems, size);
    }

#if LWMEM_CFG_FULL || __DOXYGEN__
    /**
     * \brief           Reallocate block of memory
     * \param           ptr: Pointer to previously allocated memory block
     * \param           size: Block size to allocate in units of bytes
     * \return          Allocated memory or `NULL` on failure
     * \sa              lwmem_realloc_ex
     */
    void*
    realloc(void* ptr, size_t size) {
        return lwmem_realloc_ex(&m_lw, nullptr, ptr, size);
    }

    /**
     * \brief           Free memory block
     * \param           ptr: Pointer to previously allocated memory block
     * \sa              lwmem_realloc_ex
     */
    void
    free(void* ptr) {
        lwmem_free_ex(&m_lw, ptr);
    }
#endif /* LWMEM_CFG_FULL || __DOXYGEN__ */

  private:
    /* Delete unused constructors */
    LwmemLight(const LwmemLight& other) = delete;
    /* Delete copy assignment operators */
    LwmemLight& operator=(const LwmemLight& other) = delete;
    LwmemLight* operator=(const LwmemLight* other) = delete;

    lwmem_t m_lw;
    uint8_t m_reg_data[LEN];
};

}; // namespace Lwmem

/**
 * \}
 */

#endif /* LWMEM_HDR_HPP */
