/**
 * \file            lwmem.h
 * \brief           Lightweight dynamic memory manager
 */

/*
 * Copyright (c) 2024 Tilen MAJERLE
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
 * Version:         v2.1.0
 */
#ifndef LWMEM_HDR_H
#define LWMEM_HDR_H

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include "lwmem/lwmem_opt.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup        LWMEM Lightweight dynamic memory manager
 * \brief           Lightweight dynamic memory manager
 * \{
 */

/**
 * \brief           Get size of statically allocated array
 * \param[in]       x: Object to get array size of
 * \return          Number of elements in array
 */
#define LWMEM_ARRAYSIZE(x) (sizeof(x) / sizeof((x)[0]))

/**
 * \brief           Memory block structure
 */
typedef struct lwmem_block {
    struct lwmem_block* next; /*!< Next free memory block on linked list.
                                    Set to \ref LWMEM_BLOCK_ALLOC_MARK when block is allocated and in use */
    size_t size;              /*!< Size of block, including metadata part.
                                    MSB bit is set to `1` when block is allocated and in use,
                                    or `0` when block is considered free */
} lwmem_block_t;

/**
 * \brief           Statistics structure
 */
typedef struct {
    uint32_t mem_size_bytes;                   /*!< Total memory size of all regions combined */
    uint32_t mem_available_bytes;              /*!< Free memory available for allocation */
    uint32_t minimum_ever_mem_available_bytes; /*!< Minimum amount of total free memory there has been
                                                        in the heap since the system booted. */
    uint32_t nr_alloc;                         /*!< Number of all allocated blocks in single instance  */
    uint32_t nr_free;                          /*!< Number of frees in the LwMEM instance */
} lwmem_stats_t;

/**
 * \brief           LwMEM main structure
 */
typedef struct lwmem {
    lwmem_block_t start_block;  /*!< Holds beginning of memory allocation regions */
    lwmem_block_t* end_block;   /*!< Pointer to the last memory location in regions linked list */
    size_t mem_available_bytes; /*!< Memory size available for allocation */
    size_t mem_regions_count;   /*!< Number of regions used for allocation */
#if LWMEM_CFG_OS || __DOXYGEN__
    LWMEM_CFG_OS_MUTEX_HANDLE mutex; /*!< System mutex for OS */
#endif                               /* LWMEM_CFG_OS || __DOXYGEN__ */
#if LWMEM_CFG_ENABLE_STATS || __DOXYGEN__
    lwmem_stats_t stats; /*!< Statistics */
#endif                   /* LWMEM_CFG_ENABLE_STATS || __DOXYGEN__ */
#if defined(LWMEM_DEV) && !__DOXYGEN__
    lwmem_block_t start_block_first_use; /*!< Value of start block for very first time.
                                            This is used only during validation process and is removed in final use */
#endif                                   /* defined(LWMEM_DEV) && !__DOXYGEN__ */
} lwmem_t;

/**
 * \brief           Memory region descriptor
 */
typedef struct {
    void* start_addr; /*!< Region start address */
    size_t size;      /*!< Size of region in units of bytes */
} lwmem_region_t;

size_t lwmem_assignmem_ex(lwmem_t* lwobj, const lwmem_region_t* regions);
void* lwmem_malloc_ex(lwmem_t* lwobj, const lwmem_region_t* region, const size_t size);
void* lwmem_calloc_ex(lwmem_t* lwobj, const lwmem_region_t* region, const size_t nitems, const size_t size);
#if LWMEM_CFG_SUPPORT_REALLOC_AND_FREE || __DOXYGEN__
void* lwmem_realloc_ex(lwmem_t* lwobj, const lwmem_region_t* region, void* const ptr, const size_t size);
uint8_t lwmem_realloc_s_ex(lwmem_t* lwobj, const lwmem_region_t* region, void** const ptr, const size_t size);
void lwmem_free_ex(lwmem_t* lwobj, void* const ptr);
void lwmem_free_s_ex(lwmem_t* lwobj, void** const ptr);
size_t lwmem_get_size_ex(lwmem_t* lwobj, void* ptr);
#endif /* LWMEM_CFG_SUPPORT_REALLOC_AND_FREE || __DOXYGEN__ */
#if LWMEM_CFG_ENABLE_STATS || __DOXYGEN__
void lwmem_get_stats_ex(lwmem_t* lwobj, lwmem_stats_t* stats);
#endif /* LWMEM_CFG_ENABLE_STATS || __DOXYGEN__ */

/**
 * \note            This is a wrapper for \ref lwmem_assignmem_ex function.
 *                      It operates in default LwMEM instance and uses first available region for memory operations
 * \param[in]       regions: Pointer to array of regions with address and respective size.
 *                      Regions must be in increasing order (start address) and must not overlap in-between.
 *                      Last region entry must have address `NULL` and size set to `0`
 * \code{.c}
//Example definition
lwmem_region_t regions[] = {
    { (void *)0x10000000, 0x1000 }, //Region starts at address 0x10000000 and is 0x1000 bytes long
    { (void *)0x20000000, 0x2000 }, //Region starts at address 0x20000000 and is 0x2000 bytes long
    { (void *)0x30000000, 0x3000 }, //Region starts at address 0x30000000 and is 0x3000 bytes long
    { NULL, 0 }                     //Array termination indicator
}
\endcode
 * \return          `0` on failure, number of final regions used for memory manager on success
 */
#define lwmem_assignmem(regions)   lwmem_assignmem_ex(NULL, (regions))

/**
 * \note            This is a wrapper for \ref lwmem_malloc_ex function.
 *                      It operates in default LwMEM instance and uses first available region for memory operations
 * \param[in]       size: Size to allocate in units of bytes
 * \return          Pointer to allocated memory on success, `NULL` otherwise
 * \note            This function is thread safe when \ref LWMEM_CFG_OS is enabled
 */
#define lwmem_malloc(size)         lwmem_malloc_ex(NULL, NULL, (size))

/**
 * \note            This is a wrapper for \ref lwmem_calloc_ex function.
 *                      It operates in default LwMEM instance and uses first available region for memory operations
 * \param[in]       nitems: Number of elements to be allocated
 * \param[in]       size: Size of each element, in units of bytes
 * \return          Pointer to allocated memory on success, `NULL` otherwise
 * \note            This function is thread safe when \ref LWMEM_CFG_OS is enabled
 */
#define lwmem_calloc(nitems, size) lwmem_calloc_ex(NULL, NULL, (nitems), (size))

#if LWMEM_CFG_SUPPORT_REALLOC_AND_FREE || __DOXYGEN__

/**
 * \note            This is a wrapper for \ref lwmem_realloc_ex function.
 *                      It operates in default LwMEM instance and uses first available region for memory operations
 * \param[in]       ptr: Memory block previously allocated with one of allocation functions.
 *                      It may be set to `NULL` to create new clean allocation
 * \param[in]       size: Size of new memory to reallocate
 * \return          Pointer to allocated memory on success, `NULL` otherwise
 * \note            This function is thread safe when \ref LWMEM_CFG_OS is enabled
 */
#define lwmem_realloc(ptr, size)       lwmem_realloc_ex(NULL, NULL, (ptr), (size))

/**
 * \note            This is a wrapper for \ref lwmem_realloc_s_ex function.
 *                      It operates in default LwMEM instance and uses first available region for memory operations
 * \param[in]       ptr2ptr: Pointer to pointer to allocated memory. Must not be set to `NULL`.
 *                      If reallocation is successful, it modifies pointer's pointing address,
 *                      or sets it to `NULL` in case of `free` operation
 * \param[in]       size: New requested size in bytes
 * \return          `1` if successfully reallocated, `0` otherwise
 * \note            This function is thread safe when \ref LWMEM_CFG_OS is enabled
 */
#define lwmem_realloc_s(ptr2ptr, size) lwmem_realloc_s_ex(NULL, NULL, (ptr2ptr), (size))

/**
 * \note            This is a wrapper for \ref lwmem_free_ex function.
 *                      It operates in default LwMEM instance and uses first available region for memory operations
 * \param[in]       ptr: Memory to free. `NULL` pointer is valid input
 * \note            This function is thread safe when \ref LWMEM_CFG_OS is enabled
 */
#define lwmem_free(ptr)                lwmem_free_ex(NULL, (ptr))

/**
 * \note            This is a wrapper for \ref lwmem_free_s_ex function.
 *                      It operates in default LwMEM instance and uses first available region for memory operations
 * \param[in]       ptr2ptr: Pointer to pointer to allocated memory.
 *                      When set to non `NULL`, pointer is freed and set to `NULL`
 * \note            This function is thread safe when \ref LWMEM_CFG_OS is enabled
 */
#define lwmem_free_s(ptr2ptr)          lwmem_free_s_ex(NULL, (ptr2ptr))

/**
 * \note            This is a wrapper for \ref lwmem_get_size_ex function.
 *                      It operates in default LwMEM instance and uses first available region for memory operations
 * \param[in]       ptr: Pointer to allocated memory
 * \return          Block size for user in units of bytes
 */
#define lwmem_get_size(ptr)            lwmem_get_size_ex(NULL, (ptr))

#endif /* LWMEM_CFG_SUPPORT_REALLOC_AND_FREE || __DOXYGEN__ */

#if LWMEM_CFG_ENABLE_STATS || __DOXYGEN__

/**
 * \note            This is a wrapper for \ref lwmem_get_stats_ex function.
 *                      It operates in default LwMEM instance
 * \param[in]       ptr: Pointer to lwmem_stats_t to store result
 */
#define lwmem_get_stats(stats) lwmem_get_stats_ex(NULL, (stats))

#endif /* LWMEM_CFG_ENABLE_STATS || __DOXYGEN__ */

#if defined(LWMEM_DEV) && !__DOXYGEN__
unsigned char lwmem_debug_create_regions(lwmem_region_t** regs_out, size_t count, size_t size);
void lwmem_debug_save_state(void);
void lwmem_debug_restore_to_saved(void);

void lwmem_debug_print(unsigned char print_alloc, unsigned char print_free);
#endif /* defined(LWMEM_DEV) && !__DOXYGEN__ */

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LWMEM_HDR_H */
