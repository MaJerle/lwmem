/**	
 * \file            dyn_mem.h
 * \brief           Dynamic memory manager
 */
 
/*
 * Copyright (c) 2018 Tilen Majerle
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
 * This file is part of dynamic memory library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 */
#ifndef DYN_MEM_HDR_H
#define DYN_MEM_HDR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "string.h"

/**
 * \defgroup        DYN_MEM Dynamic memory
 * \brief           Dynamic memory manager
 * \{
 */

/* --- Memory unique part starts --- */
/**
 * \brief           Memory function/typedef prefix string
 * 
 * It is used to change function names in zero time to easily re-use same library between applications.
 * Use `#define MEM_PREF(x)    my_prefix_ ## x` to change all function names to (for example) `my_prefix_mem_init` 
 *
 * \note            Modification of this macro must be done in header and source file aswell
 */
#define MEM_PREF(x)                     x
/* --- Memory unique part ends --- */

/**
 * \brief           Memory region descriptor
 */
typedef struct {   
    void* start_addr;                           /*!< Region start address */
    size_t size;                                /*!< Size of region in units of bytes */
} MEM_PREF(mem_region_t);

size_t      MEM_PREF(mem_assignmem)(const MEM_PREF(mem_region_t)* regions, const size_t len);
size_t      MEM_PREF(mem_init)(const MEM_PREF(mem_region_t)* regions, const size_t len);
void *      MEM_PREF(mem_malloc)(const size_t size);
void *      MEM_PREF(mem_calloc)(const size_t nitems, const size_t size);
void *      MEM_PREF(mem_realloc)(void* const ptr, const size_t size);
void        MEM_PREF(mem_free)(void* const ptr);

#undef MEM_PREF

/**
 * \}
 */

#ifdef __cplusplus
}
#endif

#endif /* DYN_MEM_HDR_H */