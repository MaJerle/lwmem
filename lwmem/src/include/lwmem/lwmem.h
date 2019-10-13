/**	
 * \file            lwmem.c
 * \brief           Lightweight dynamic memory manager
 */
 
/*
 * Copyright (c) 2019 Tilen MAJERLE
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
 * Version:         $_version_$
 */
#ifndef LWMEM_HDR_H
#define LWMEM_HDR_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <string.h>
#include "lwmem_config.h"

/**
 * \defgroup        LWMEM Lightweight dynamic memory manager
 * \brief           Lightweight dynamic memory manager
 * \{
 */

/* --- Memory unique part starts --- */
/**
 * \brief           Memory function/typedef prefix string
 * 
 * It is used to change function names in zero time to easily re-use same library between applications.
 * Use `#define LWMEM_PREF(x)   my_prefix_ ## x` to change all function names to (for example) `my_prefix_lwmem_init` 
 *
 * \note            Modification of this macro must be done in header and source file aswell
 */
#define LWMEM_PREF(x)                     lwmem_ ## x
/* --- Memory unique part ends --- */

/**
 * \brief           Memory region descriptor
 */
typedef struct {
    void* start_addr;                           /*!< Region start address */
    size_t size;                                /*!< Size of region in units of bytes */
} LWMEM_PREF(region_t);

size_t          LWMEM_PREF(assignmem)(const LWMEM_PREF(region_t)* regions, const size_t len);
void *          LWMEM_PREF(malloc)(const size_t size);
void *          LWMEM_PREF(calloc)(const size_t nitems, const size_t size);
void *          LWMEM_PREF(realloc)(void* const ptr, const size_t size);
unsigned char   LWMEM_PREF(realloc_s)(void** const ptr, const size_t size);
void            LWMEM_PREF(free)(void* const ptr);
void            LWMEM_PREF(free_s)(void** const ptr);

#if defined(LWMEM_DEV) && !__DOXYGEN__
unsigned char lwmem_debug_create_regions(LWMEM_PREF(region_t)** regs_out, size_t count, size_t size);
void    lwmem_debug_save_state(void);
void    lwmem_debug_restore_to_saved(void);

void    lwmem_debug_print(unsigned char print_alloc, unsigned char print_free);
#endif /* defined(LWMEM_DEV) && !__DOXYGEN__ */

#undef LWMEM_PREF

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LWMEM_HDR_H */