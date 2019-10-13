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
#include "lwmem/lwmem.h"
#include <limits.h>

#if LWMEM_CFG_OS
#include "system/lwmem_sys.h"
#endif /* LWMEM_CFG_OS */

/* --- Memory unique part starts --- */
/* Prefix for all buffer functions and typedefs */
/**
 * \brief           Memory function/typedef prefix string
 */
#define LWMEM_PREF(x)                   lwmem_ ## x

/**
 * \brief           Number of bits to align memory address and size.
 *
 * Most CPUs do not offer unaligned memory access (Cortex-M0 as an example)
 * therefore it is important to have alignment.
 *
 * \note            This value can be a power of `2`. Usually alignment of `4` bytes fits to all processors.
 */
#define LWMEM_ALIGN_NUM                 ((size_t)4)

#define LWMEM_MEMSET                    memset
#define LWMEM_MEMCPY                    memcpy
#define LWMEM_MEMMOVE                   memmove
/* --- Memory unique part ends --- */

/**
 * \brief           Transform alignment number (power of `2`) to bits
 */
#define LWMEM_ALIGN_BITS                ((size_t)(LWMEM_ALIGN_NUM - 1))

/**
 * \brief           Aligns input value to next alignment bits
 *
 * As an example, when \ref MEM_ALIGN_NUM is set to `4`:
 *
 *  - Input: `0`; Output: `0`
 *  - Input: `1`; Output: `4`
 *  - Input: `2`; Output: `4`
 *  - Input: `3`; Output: `4`
 *  - Input: `4`; Output: `4`
 *  - Input: `5`; Output: `8`
 *  - Input: `6`; Output: `8`
 *  - Input: `7`; Output: `8`
 *  - Input: `8`; Output: `8`
 */
#define LWMEM_ALIGN(x)                  (((x) + (LWMEM_ALIGN_BITS)) & ~(LWMEM_ALIGN_BITS))

/**
 * \brief           Size of metadata header for block information
 */
#define LWMEM_BLOCK_META_SIZE           LWMEM_ALIGN(sizeof(lwmem_block_t))

/**
 * \brief           Cast input pointer to byte
 * \param[in]       p: Input pointer to cast to byte pointer
 */
#define LWMEM_TO_BYTE_PTR(p)            ((unsigned char *)(p))

/**
 * \brief           Bit indicating memory block is allocated
 */
#define LWMEM_ALLOC_BIT                 ((size_t)((size_t)1 << (sizeof(size_t) * CHAR_BIT - 1)))

/**
 * \brief           Mark written in `next` field when block is allocated
 */
#define LWMEM_BLOCK_ALLOC_MARK          (0xDEADBEEF)

/**
 * \brief           Set block as allocated
 * \param[in]       block: Block to set as allocated
 */
#define LWMEM_BLOCK_SET_ALLOC(block)    do { if ((block) != NULL) { (block)->size |= LWMEM_ALLOC_BIT; (block)->next = (void *)(LWMEM_TO_BYTE_PTR(0) + LWMEM_BLOCK_ALLOC_MARK); }} while (0)

/**
 * \brief           Check if input block is properly allocated and valid
 * \param[in]       block: Block to check if properly set as allocated
 */
#define LWMEM_BLOCK_IS_ALLOC(block)     ((block) != NULL && ((block)->size & LWMEM_ALLOC_BIT) && (block)->next == (void *)(LWMEM_TO_BYTE_PTR(0) + LWMEM_BLOCK_ALLOC_MARK))

/**
 * \brief           Get block handle from application pointer
 * \param[in]       ptr: Input pointer to get block from
 */
#define LWMEM_GET_BLOCK_FROM_PTR(ptr)   (void *)((ptr) != NULL ? ((LWMEM_TO_BYTE_PTR(ptr)) - LWMEM_BLOCK_META_SIZE) : NULL)

/**
 * \brief           Get block handle from application pointer
 * \param[in]       ptr: Input pointer to get block from
 */
#define LWMEM_GET_PTR_FROM_BLOCK(block) (void *)((block) != NULL ? ((LWMEM_TO_BYTE_PTR(block)) + LWMEM_BLOCK_META_SIZE) : NULL)

/**
 * \brief           Minimum amount of memory required to make new empty block
 *
 * Default size is size of meta block
 */
#define LWMEM_BLOCK_MIN_SIZE            (LWMEM_BLOCK_META_SIZE)

/**
 * \brief           Gets block before input block (marked as prev) and its previous free block
 * \param[in]       _b_: Input block to find previous and its previous
 * \param[in]       _pp_: Previous previous of input block
 * \param[in]       _p_: Previous of input block
 */
#define LWMEM_GET_PREV_CURR_OF_BLOCK(_b_, _pp_, _p_) do {               \
    for ((_pp_) = NULL, (_p_) = &lwmem.start_block;                     \
        (_p_) != NULL && (_p_)->next < (_b_);                           \
        (_pp_) = (_p_), (_p_) = (_p_)->next                             \
    ) {}                                                                \
} while (0)

#if LWMEM_CFG_OS
#define LWMEM_PROTECT()         lwmem_sys_mutex_wait(&mutex)
#define LWMEM_UNPROTECT()       lwmem_sys_mutex_release(&mutex)
#else /* LWMEM_CFG_OS */
#define LWMEM_PROTECT()
#define LWMEM_UNPROTECT()
#endif /* !LWMEM_CFG_OS */

/**
 * \brief           Memory block structure
 */
typedef struct lwmem_block {
    struct lwmem_block* next;                   /*!< Next free memory block on linked list.
                                                        Set to \ref LWMEM_BLOCK_ALLOC_MARK when block is allocated and in use */
    size_t size;                                /*!< Size of block. MSB bit is set to `1` when block is allocated and in use,
                                                        or `0` when block is free */
} lwmem_block_t;

/**
 * \brief           Lwmem main structure
 */
typedef struct lwmem {
    lwmem_block_t start_block;                  /*!< Holds beginning of memory allocation regions */
    lwmem_block_t* end_block;                   /*!< Pointer to the last memory location in regions linked list */
    size_t mem_available_bytes;                 /*!< Memory size available for allocation */
    size_t mem_regions_count;                   /*!< Number of regions used for allocation */
#if defined(LWMEM_DEV) && !__DOXYGEN__
    lwmem_block_t start_block_first_use;        /*!< Value of start block for very first time */
#endif /* defined(LWMEM_DEV) && !__DOXYGEN__ */
} lwmem_t;

/**
 * \brief           Lwmem data
 */
static lwmem_t lwmem;
#if LWMEM_CFG_OS || __DOXYGEN__
static LWMEM_CFG_OS_MUTEX_HANDLE mutex;         /*!< System mutex */
#endif /* LWMEM_CFG_OS || __DOXYGEN__ */

/**
 * \brief           Insert free block to linked list of free blocks
 * \param[in]       nb: New free block to insert into linked list
 */
static void
prv_insert_free_block(lwmem_block_t* nb) {
    lwmem_block_t* prev;

    /* 
     * Try to find position to put new block in-between
     * Search until all free block addresses are lower than entry block
     */
    for (prev = &lwmem.start_block; prev != NULL && prev->next < nb; prev = prev->next) {}

    /* This is hard error with wrong memory usage */
    if (prev == NULL) {
        return;
    }

    /*
     * At this point we have valid previous block
     * Previous block is last free block before input block
     */

    /*
     * Check if previous block and input block together create one big contiguous block
     * If this is the case, merge blocks together and increase previous block by input block size
     */
    if ((LWMEM_TO_BYTE_PTR(prev) + prev->size) == LWMEM_TO_BYTE_PTR(nb)) {
        prev->size += nb->size;                 /* Increase current block by size of new block */
        nb = prev;                              /* New block and current are now the same thing */
        /* 
         * It is important to set new block as current one
         * as this allows merging previous and next blocks together with new block
         * at the same time; follow next steps
         */
    }

    /* 
     * Check if new block and next of previous create big contiguous block
     * Do not merge with "end of region" indication (commented part of if statement)
     */
    if (prev->next != NULL && prev->next->size > 0  /* Do not remove "end of region" indicator in each region */
        && (LWMEM_TO_BYTE_PTR(nb) + nb->size) == LWMEM_TO_BYTE_PTR(prev->next)) {
        if (prev->next == lwmem.end_block) {    /* Does it points to the end? */
            nb->next = lwmem.end_block;         /* Set end block pointer */
        } else {
            nb->size += prev->next->size;       /* Expand of current block for size of next free block which is right behind new block */
            nb->next = prev->next->next;        /* Next free is pointed to the next one of previous next */
        }
    } else {
        nb->next = prev->next;                  /* Set next of input block as next of current one */
    }

    /*
     * If new block has not been set as current (and expanded),
     * then link them together, otherwise ignore as it would point to itself
     */
    if (prev != nb) {
        prev->next = nb;
    }
}

/**
 * \brief           Split too big block and add it to list of free blocks
 * \param[in]       block: Pointer to block with size already set
 * \param[in]       new_block_size: New block size to be set
 * \return          `1` if block splitted, `0` otherwise
 */
static unsigned char
prv_split_too_big_block(lwmem_block_t* block, size_t new_block_size) {
    lwmem_block_t* next;
    size_t block_size, is_alloc_bit;
    unsigned char success = 0;
    
    is_alloc_bit = block->size & LWMEM_ALLOC_BIT;   /* Check if allocation bit is set */
    block_size = block->size & ~LWMEM_ALLOC_BIT;/* Use size without allocated bit */

    /*
     * If current block size is greater than requested size,
     * it is possible to create empty block at the end of existing one
     * and add it back to list of empty blocks
     */
    if ((block_size - new_block_size) >= LWMEM_BLOCK_MIN_SIZE) {
        next = (void *)(LWMEM_TO_BYTE_PTR(block) + new_block_size); /* Put next block after size of current allocation */
        next->size = block_size - new_block_size;   /* Modify block data */
        block->size = new_block_size;           /* Current size is now smaller */

        lwmem.mem_available_bytes += next->size;/* Increase available bytes by new block size */
        prv_insert_free_block(next);            /* Add new block to the free list */

        success = 1;
    } else {
        /* TODO: If next of current is free, check if we can increase next by at least some bytes */
        /* This can only happen during reallocation process when allocated block is reallocated to previous one */
        /* Very rare case, but may happen! */
    }
    
    /* If allocation bit was set before, set it now again */
    if (is_alloc_bit) {
        LWMEM_BLOCK_SET_ALLOC(block);
    }
    return success;
}

/**
 * \brief           Private allocation function
 * \param[in]       ptr: Pointer to already allocated memory, used in case of memory expand (realloc) feature.
 *                      It shall point to beginning of meta block when used.
 *                      Set to `NULL` for new allocation.
 *                      If parameter is not `NULL` and function returns `NULL`, memory is not automatically freed
 * \param[in]       size: Application wanted size, excluding size of meta header
 * \return          Pointer to allocated memory, `NULL` otherwise
 */
static void *
prv_alloc(const size_t size) {
    lwmem_block_t* prev, *curr;
    void* retval = NULL;

    /* Calculate final size including meta data size */
    const size_t final_size = LWMEM_ALIGN(size) + LWMEM_BLOCK_META_SIZE;

    /* Check if initialized and if size is in the limits */
    if (lwmem.end_block == NULL || final_size == LWMEM_BLOCK_META_SIZE || (final_size & LWMEM_ALLOC_BIT)) {
        return NULL;
    }

    /* Try to find first block with at least `size` bytes of available memory */
    for (prev = &lwmem.start_block, curr = prev->next;  /* Start from very beginning and set curr as first empty block */
        curr != NULL && curr->size < final_size;/* Loop until block size is smaller than requested */
        prev = curr, curr = curr->next) {       /* Go to next free block */
        if (curr->next == NULL || curr == lwmem.end_block) {/* If no more blocks available */
            return NULL;                        /* No sufficient memory available to allocate block of memory */
        }
    }

    /* Check curr pointer. During normal use, this should be always false */
    if (curr == NULL) {
        return NULL;
    }

    /* There is a valid block available */
    retval = LWMEM_GET_PTR_FROM_BLOCK(curr);    /* Return pointer does not include meta part */
    prev->next = curr->next;                    /* Remove this block from linked list by setting next of previous to next of current */

    /* curr block is now removed from linked list */
    
    lwmem.mem_available_bytes -= curr->size;    /* Decrease available bytes by allocated block size */
    prv_split_too_big_block(curr, final_size);  /* Split block if it is too big */
    LWMEM_BLOCK_SET_ALLOC(curr);                /* Set block as allocated */

    return retval;
}

/**
 * \brief           Free input pointer
 * \param[in]       ptr: Input pointer to free
 */
void
prv_free(void* const ptr) {
    lwmem_block_t* const block = LWMEM_GET_BLOCK_FROM_PTR(ptr);
    if (LWMEM_BLOCK_IS_ALLOC(block)) {          /* Check if block is valid */
        block->size &= ~LWMEM_ALLOC_BIT;        /* Clear allocated bit indication */

        lwmem.mem_available_bytes += block->size;   /* Increase available bytes */
        prv_insert_free_block(block);           /* Put block back to list of free block */
    }
}

/**
 * \brief           Initializes and assigns user regions for memory used by allocator algorithm
 * \param[in]       regions: Array of regions with address and its size.
 *                      Regions must be in increasing order (start address) and must not overlap in-between
 * \param[in]       len: Number of regions in array
 * \return          `0` on failure, number of final regions used for memory manager on success
 * \note            This function is not thread safe when used with operating system.
 *                  It must be called only once to setup memory regions
 */
size_t
LWMEM_PREF(assignmem)(const LWMEM_PREF(region_t)* regions, const size_t len) {
    unsigned char* mem_start_addr;
    size_t mem_size;
    lwmem_block_t* first_block, *prev_end_block;

    if (lwmem.end_block != NULL                 /* Init function may only be called once */
        || (LWMEM_ALIGN_NUM & (LWMEM_ALIGN_NUM - 1))/* Must be power of 2 */
        || regions == NULL || len == 0
#if LWMEM_CFG_OS
        || lwmem_sys_mutex_isvalid(&mutex)      /* Check if mutex valid already */
#endif /* LWMEM_CFG_OS */
        ) {       /* Check inputs */
        return 0;
    }

#if LWMEM_CFG_OS
    if (!lwmem_sys_mutex_create(&mutex)) {
        return 0;
    }
#endif /* LWMEM_CFG_OS */

    /* Ensure regions are growing linearly and do not overlap in between */
    mem_start_addr = (void *)0;
    mem_size = 0;
    for (size_t i = 0; i < len; i++) {
        /* New region(s) must be higher (in address space) than previous one */
        if ((mem_start_addr + mem_size) > LWMEM_TO_BYTE_PTR(regions[i].start_addr)) {
            return 0;
        }

        /* Save new values for next try */
        mem_start_addr = regions[i].start_addr;
        mem_size = regions[i].size;
    }

    for (size_t i = 0; i < len; i++, regions++) {
        /* 
         * Check region start address and align start address accordingly
         * It is ok to cast to size_t, even if pointer could be larger
         * Important is to check lower-bytes (and bits)
         */
        mem_size = regions->size & ~LWMEM_ALIGN_BITS;   /* Size does not include lower bits */
        if (mem_size < (2 * LWMEM_BLOCK_MIN_SIZE)) {
            continue;                           /* Ignore region, go to next one */
        }

        /*
         * Start address must be aligned to configuration
         * Increase start address and decrease effective region size
         */
        mem_start_addr = regions->start_addr;
        if (((size_t)mem_start_addr) & LWMEM_ALIGN_BITS) {  /* Check alignment boundary */
            mem_start_addr += LWMEM_ALIGN_NUM - ((size_t)mem_start_addr & LWMEM_ALIGN_BITS);
            mem_size -= mem_start_addr - LWMEM_TO_BYTE_PTR(regions->start_addr);
        }
        
        /* Ensure region size has enough memory after all the alignment checks */
        if (mem_size < (2 * LWMEM_BLOCK_MIN_SIZE)) {
            continue;                           /* Ignore region, go to next one */
        }

        /*
         * If end_block == NULL, this indicates first iteration.
         * In first indication application shall set start_block and never again
         * end_block value holds
         */
        if (lwmem.end_block == NULL) {
            /*
             * Next entry of start block is first region
             * It points to beginning of region data
             * In the later step(s) first block is manually set on top of memory region
             */
            lwmem.start_block.next = (void *)mem_start_addr;
            lwmem.start_block.size = 0;         /* Size of dummy start block is zero */
        }

        /* Save current end block status as it is used later for linked list insertion */
        prev_end_block = lwmem.end_block;

        /* Put end block to the end of the region with size = 0 */
        lwmem.end_block = (void *)(mem_start_addr + mem_size - LWMEM_BLOCK_META_SIZE);
        lwmem.end_block->next = NULL;           /* End block in region does not have next entry */
        lwmem.end_block->size = 0;              /* Size of end block is zero */

        /*
         * Create memory region first block.
         *
         * First block meta size includes size of metadata too
         * Subtract MEM_BLOCK_META_SIZE as there is one more block (end_block) at the end of region
         *
         * Actual maximal available size for application in the region is mem_size - 2 * MEM_BLOCK_META_SIZE
         */
        first_block = (void *)mem_start_addr;
        first_block->next = lwmem.end_block;    /* Next block of first is last block */
        first_block->size = mem_size - LWMEM_BLOCK_META_SIZE;

        /* Check if previous regions exist by checking previous end block state */
        if (prev_end_block != NULL) {
            prev_end_block->next = first_block; /* End block of previous region now points to start of current region */
        }

        lwmem.mem_available_bytes += first_block->size; /* Increase number of available bytes */
        lwmem.mem_regions_count++;              /* Increase number of used regions */
    }

#if defined(LWMEM_DEV)
    /* Copy default state of start block */
    LWMEM_MEMCPY(&lwmem.start_block_first_use, &lwmem.start_block, sizeof(lwmem.start_block));
#endif /* defined(LWMEM_DEV) */

    return lwmem.mem_regions_count;             /* Return number of regions used by manager */
}

/**
 * \brief           Allocate memory of requested size
 * \note            Function declaration is in-line with standard C function `malloc`
 * \param[in]       size: Number of bytes to allocate
 * \return          Pointer to allocated memory on success, `NULL` otherwise
 * \note            This function is thread safe when \ref LWMEM_CFG_OS is enabled
 */
void *
LWMEM_PREF(malloc)(const size_t size) {
    void* ptr;
    LWMEM_PROTECT();
    ptr = prv_alloc(size);
    LWMEM_UNPROTECT();
    return ptr;
}

/**
 * \brief           Allocate contiguous block of memory for requested number of items and its size.
 *
 * It resets allocated block of memory to zero if allocation is successful
 *
 * \note            Function declaration is in-line with standard C function `calloc`
 * \param[in]       nitems: Number of elements to be allocated
 * \param[in]       size: Size of each element, in units of bytes
 * \return          Pointer to allocated memory on success, `NULL` otherwise
 * \note            This function is thread safe when \ref LWMEM_CFG_OS is enabled
 */
void *
LWMEM_PREF(calloc)(const size_t nitems, const size_t size) {
    void* ptr;
    const size_t s = size * nitems;

    LWMEM_PROTECT();
    if ((ptr = prv_alloc(s)) != NULL) {
        LWMEM_MEMSET(ptr, 0x00, s);
    }
    LWMEM_UNPROTECT();
    return ptr;
}

/**
 * \brief           Reallocates already allocated memory with new size
 *
 * Function behaves differently, depends on input parameter of `ptr` and `size`:
 *
 *  - `ptr == NULL; size == 0`: Function returns `NULL`, no memory is allocated or freed
 *  - `ptr == NULL; size > 0`: Function tries to allocate new block of memory with `size` length, equivalent to `malloc(size)`
 *  - `ptr != NULL; size == 0`: Function frees memory, equivalent to `free(ptr)`
 *  - `ptr != NULL; size > 0`: Function tries to allocate new memory of copy content before returning pointer on success
 *
 * \note            Function declaration is in-line with standard C function `realloc`
 *
 * \param[in]       ptr: Memory block previously allocated with one of allocation functions.
 *                      It may be set to `NULL` to create new clean allocation
 * \param[in]       size: Size of new memory to reallocate
 * \return          Pointer to allocated memory on success, `NULL` otherwise
 * \note            This function is thread safe when \ref LWMEM_CFG_OS is enabled
 */
void *
LWMEM_PREF(realloc)(void* const ptr, const size_t size) {
    lwmem_block_t* block, *prevprev, *prev;
    size_t block_size;                          /* Holds size of input block (ptr), including metadata size */
    const size_t final_size = LWMEM_ALIGN(size) + LWMEM_BLOCK_META_SIZE;/* Holds size of new requested block size, including metadata size */
    void* retval;                               /* Return pointer, used with LWMEM_RETURN macro */

    /* Protect lwmem core */
    #define LWMEM_RETURN(x)     do { retval = (x); goto ret; } while (0)
    LWMEM_PROTECT();

    /* Check optional input parameters */
    if (size == 0) {
        if (ptr != NULL) {
            prv_free(ptr);
        }
        LWMEM_RETURN(NULL);
    }
    if (ptr == NULL) {
        LWMEM_RETURN(prv_alloc(size));
    }

    /* Try to reallocate existing pointer */
    if ((size & LWMEM_ALLOC_BIT) || (final_size & LWMEM_ALLOC_BIT)) {
        LWMEM_RETURN(NULL);
    }

    /* Process existing block */
    block = LWMEM_GET_BLOCK_FROM_PTR(ptr);
    if (LWMEM_BLOCK_IS_ALLOC(block)) {
        block_size = block->size & ~LWMEM_ALLOC_BIT;/* Get actual block size, without memory allocation bit */

        /* Check current block size is the same as new requested size */
        if (block_size == final_size) {
            LWMEM_RETURN(ptr);                  /* Just return pointer, nothing to do */
        }

        /*
         * Abbreviations
         *
         * - "Current block" or "Input block" is allocated block from input variable "ptr"
         * - "Next free block" is next free block, on address space after input block
         * - "Prev free block" is last free block, on address space before input block
         * - "PrevPrev free block" is previous free block of "Prev free block"
         */

        /*
         * When new requested size is smaller than existing one,
         * it is enough to modify size of current block only.
         *
         * If new requested size is much smaller than existing one,
         * check if it is possible to create new empty block and add it to list of empty blocks
         *
         * Application returns same pointer
         */
        if (final_size < block_size) {
            if ((block_size - final_size) >= LWMEM_BLOCK_MIN_SIZE) {
                prv_split_too_big_block(block, final_size); /* Split block if it is too big */
            } else {
                /*
                 * It is not possible to create new empty block at the end of input block
                 * 
                 * But if next free block is just after input block,
                 * it is possible to find this block and increase it by "block_size - final_size" bytes
                 */

                /* Find free blocks before input block */
                LWMEM_GET_PREV_CURR_OF_BLOCK(block, prevprev, prev);

                /* Check if current block and next free are connected */
                if ((LWMEM_TO_BYTE_PTR(block) + block_size) == LWMEM_TO_BYTE_PTR(prev->next)
                    && prev->next->size > 0) {  /* Must not be end of region indicator */
                    /* Make temporary variables as prev->next will point to different location */
                    const size_t tmp_size = prev->next->size;
                    void* const tmp_next = prev->next->next;

                    /* Shift block up, effectively increase its size */
                    prev->next = (void *)(LWMEM_TO_BYTE_PTR(prev->next) - (block_size - final_size));
                    prev->next->size = tmp_size + (block_size - final_size);
                    prev->next->next = tmp_next;
                    lwmem.mem_available_bytes += block_size - final_size;   /* Increase available bytes by increase of free block */

                    block->size = final_size;   /* Block size is requested size */
                }
            }
            LWMEM_BLOCK_SET_ALLOC(block);       /* Set block as allocated */
            LWMEM_RETURN(ptr);                  /* Return existing pointer */
        }

        /* New requested size is bigger than current block size is */

        /* Find last free (and its previous) block, located just before input block */
        LWMEM_GET_PREV_CURR_OF_BLOCK(block, prevprev, prev);

        /* If entry could not be found, there is a hard error */
        if (prev == NULL) {
            LWMEM_RETURN(NULL);
        }
        
        /* Order of variables is: | prevprev ---> prev --->--->--->--->--->--->--->--->--->---> prev->next  | */
        /*                        |                      (input_block, which is not on a list)              | */
        /* Input block points to address somewhere between "prev" and "prev->next" pointers                   */

        /* Check if "block" and next free "prev->next" create contiguous memory with size of at least new requested size */
        if ((LWMEM_TO_BYTE_PTR(block) + block_size) == LWMEM_TO_BYTE_PTR(prev->next)/* Blocks create contiguous block */
            && (block_size + prev->next->size) >= final_size) { /* Size is greater or equal to requested */

            /*
             * Merge blocks together by increasing current block with size of next free one
             * and remove next free from list of free blocks
             */
            lwmem.mem_available_bytes -= prev->next->size;  /* For now decrease effective available bytes */
            block->size = block_size + prev->next->size;/* Increase effective size of new block */
            prev->next = prev->next->next;      /* Set next to next's next, effectively remove expanded block from free list */

            prv_split_too_big_block(block, final_size); /* Split block if it is too big */
            LWMEM_BLOCK_SET_ALLOC(block);       /* Set block as allocated */
            LWMEM_RETURN(ptr);                  /* Return existing pointer */
        }

        /* 
         * Check if "block" and last free before "prev" create contiguous memory with size of at least new requested size.
         *
         * It is necessary to make a memory move and shift content up as new return pointer is now upper on address space
         */
        if ((LWMEM_TO_BYTE_PTR(prev) + prev->size) == LWMEM_TO_BYTE_PTR(block)  /* Blocks create contiguous block */
            && (prev->size + block_size) >= final_size) {   /* Size is greater or equal to requested */
            /* Move memory from block to block previous to current */
            void* const old_data_ptr = LWMEM_GET_PTR_FROM_BLOCK(block);
            void* const new_data_ptr = LWMEM_GET_PTR_FROM_BLOCK(prev);
            LWMEM_MEMMOVE(new_data_ptr, old_data_ptr, block_size);  /* Copy old buffer size to new location */

            /*
             * If memmove overwrites metadata of current block (when shifting content up),
             * it is not an issue as we know its size and next is already NULL
             */

            lwmem.mem_available_bytes -= prev->size;/* For now decrease effective available bytes */
            prev->size += block_size;           /* Increase size of input block size */
            prevprev->next = prev->next;        /* Remove prev from free list as it is now being used for allocation together with existing block */
            block = prev;                       /* Move block pointer to previous one */

            prv_split_too_big_block(block, final_size); /* Split block if it is too big */
            LWMEM_BLOCK_SET_ALLOC(block);       /* Set block as allocated */
            LWMEM_RETURN(new_data_ptr);         /* Return new data ptr */
        }

        /*
         * At this point, it was not possible to expand existing block with free before or free after due to:
         * - Input block & next free block do not create contiguous block or its new size is too small
         * - Last free block & input block do not create contiguous block or its new size is too small
         *
         * Last option is to check if last free block before "prev", input block "block" and next free block "prev->next" create contiguous block
         * and size of new block (from 3 contiguous blocks) together is big enough
         */
        if ((LWMEM_TO_BYTE_PTR(prev) + prev->size) == LWMEM_TO_BYTE_PTR(block)  /* Input block and free block before create contiguous block */
            && (LWMEM_TO_BYTE_PTR(block) + block_size) == LWMEM_TO_BYTE_PTR(prev->next) /* Input block and free block after create contiguous block */
            && (prev->size + block_size + prev->next->size) >= final_size) {/* Size is greater or equal to requested */

            /* Move memory from block to block previous to current */
            void* const old_data_ptr = LWMEM_GET_PTR_FROM_BLOCK(block);
            void* const new_data_ptr = LWMEM_GET_PTR_FROM_BLOCK(prev);

            /*
             * It is necessary to use memmove and not memcpy as memmove takes care of memory overlapping
             * It is not a problem if data shifted up overwrite old block metadata
             */
            LWMEM_MEMMOVE(new_data_ptr, old_data_ptr, block_size);  /* Copy old buffer size to new location */

            lwmem.mem_available_bytes -= prev->size + prev->next->size; /* Decrease effective available bytes for free blocks before and after input block */
            prev->size += block_size + prev->next->size;/* Increase size of new block by size of 2 free blocks */
            prevprev->next = prev->next->next;  /* Remove free block before current one and block after current one from linked list (remove 2) */
            block = prev;                       /* Previous block is now current */

            prv_split_too_big_block(block, final_size); /* Split block if it is too big */
            LWMEM_BLOCK_SET_ALLOC(block);       /* Set block as allocated */
            LWMEM_RETURN(new_data_ptr);         /* Return new data ptr */
        }
    } else {
        /* Hard error. Input pointer is not NULL and block is not considered allocated */
        LWMEM_RETURN(NULL);
    }

    /*
     * If application reaches this point, it means:
     * - New requested size is greater than input block size
     * - Input block & next free block do not create contiguous block or its new size is too small
     * - Last free block & input block do not create contiguous block or its new size is too small
     * - Last free block & input block & next free block do not create contiguous block or its size is too small
     *
     * Final solution is to find completely new empty block of sufficient size and copy content from old one to new one
     */
    retval = prv_alloc(size);                   /* Try to allocate new block */
    if (retval != NULL) {
        block_size = (block->size & ~LWMEM_ALLOC_BIT) - LWMEM_BLOCK_META_SIZE;  /* Get application size from input pointer */
        LWMEM_MEMCPY(retval, ptr, size > block_size ? block_size : size);   /* Copy content to new allocated block */
        prv_free(ptr);                          /* Free input pointer */
    }
    LWMEM_RETURN(retval);

ret:
    LWMEM_UNPROTECT();
    return retval;
}

/**
 * \brief           Safe version of classic realloc function
 *
 * It is advised to use this function when reallocating memory.
 * After memory is reallocated, input pointer automatically points to new memory
 * to prevent use of dangling pointers.
 *
 * Function behaves differently, depends on input parameter of `ptr` and `size`:
 *
 *  - `ptr == NULL`: Invalid input, function returns `0`
 *  - `*ptr == NULL; size == 0`: Function returns `0`, no memory is allocated or freed
 *  - `*ptr == NULL; size > 0`: Function tries to allocate new block of memory with `size` length, equivalent to `malloc(size)`
 *  - `*ptr != NULL; size == 0`: Function frees memory, equivalent to `free(ptr)`, sets input pointer pointing to `NULL`
 *  - `*ptr != NULL; size > 0`: Function tries to reallocate existing pointer with new size and copy content to new block
 *
 * \param[in]       ptr: Pointer to pointer to allocated memory. Must not be set to `NULL`.
 *                      If reallocation is successful, it modified where pointer points to,
 *                      or sets it to `NULL` in case of `free` operation
 * \param[in]       size: New requested size
 * \return          `1` if successfully reallocated, `0` otherwise
 * \note            This function is thread safe when \ref LWMEM_CFG_OS is enabled
 */
unsigned char
LWMEM_PREF(realloc_s)(void** const ptr, const size_t size) {
    void* new_ptr;

    /*
     * Input pointer must not be NULL otherwise,
     * in case of successful allocation, we have memory leakage
     * aka. allocated memory where noone is pointing to it
     */
    if (ptr == NULL) {
        return 0;
    }
    
    new_ptr = LWMEM_PREF(realloc)(*ptr, size);  /* Try to reallocate existing pointer */
    if (new_ptr != NULL) {
        *ptr = new_ptr;
    } else if (size == 0) {                     /* size == 0 means free input memory */
        *ptr = NULL;
        return 1;
    }
    return new_ptr != NULL;
}

/**
 * \brief           Free previously allocated memory using one of allocation functions
 * \note            Function declaration is in-line with standard C function `free`
 * \param[in]       ptr: Memory to free. `NULL` pointer is valid input
 * \note            This function is thread safe when \ref LWMEM_CFG_OS is enabled
 */
void
LWMEM_PREF(free)(void* const ptr) {
    LWMEM_PROTECT();
    prv_free(ptr);                              /* Free pointer */
    LWMEM_UNPROTECT();
}

/**
 * \brief           Safe version of free function
 * 
 * It is advised to use this function when freeing memory.
 * After memory is freed, input pointer is safely set to `NULL`
 * to prevent use of dangling pointers.
 *
 * \param[in]       ptr: Pointer to pointer to allocated memory.
 *                  When set to non `NULL`, pointer is freed and set to `NULL`
 * \note            This function is thread safe when \ref LWMEM_CFG_OS is enabled
 */
void
LWMEM_PREF(free_s)(void** const ptr) {
    if (ptr != NULL && *ptr != NULL) {
        LWMEM_PROTECT();
        prv_free(*ptr);
        LWMEM_UNPROTECT();
        *ptr = NULL;
    }
}

/* Part of library used ONLY for LWMEM_DEV purposes */
/* To validate and test library */

#if defined(LWMEM_DEV) && !__DOXYGEN__

#include <stdio.h>
#include <stdlib.h>

/* Temporary variable for lwmem save */
static lwmem_t lwmem_temp;
static lwmem_region_t* regions_orig;
static lwmem_region_t* regions_temp;
static size_t regions_count;

static LWMEM_PREF(region_t) *
create_regions(size_t count, size_t size) {
    LWMEM_PREF(region_t)* regions;
    LWMEM_PREF(region_t) tmp;

    /* Allocate pointer structure */
    regions = malloc(count * sizeof(*regions));
    if (regions == NULL) {
        return NULL;
    }

    /* Allocate memory for regions */
    for (size_t i = 0; i < count; i++) {
        regions[i].size = size;
        regions[i].start_addr = malloc(regions[i].size);
        if (regions[i].start_addr == NULL) {
            return NULL;
        }
    }

    /* Sort regions, make sure they grow linearly */
    for (size_t x = 0; x < count; x++) {
        for (size_t y = 0; y < count; y++) {
            if (regions[x].start_addr < regions[y].start_addr) {
                memcpy(&tmp, &regions[x], sizeof(regions[x]));
                memcpy(&regions[x], &regions[y], sizeof(regions[x]));
                memcpy(&regions[y], &tmp, sizeof(regions[x]));
            }
        }
    }

    return regions;
}

static void
print_block(size_t i, lwmem_block_t* block) {
    size_t is_free, block_size;

    is_free = (block->size & LWMEM_ALLOC_BIT) == 0 && block != &lwmem.start_block_first_use && block->size > 0;
    block_size = block->size & ~LWMEM_ALLOC_BIT;

    printf("| %5d | %12p | %6d | %4d | %16d |",
        (int)i,
        block,
        (int)is_free,
        (int)block_size,
        (int)(is_free ? (block_size - LWMEM_BLOCK_META_SIZE) : 0));
    if (block == &lwmem.start_block_first_use) {
        printf(" Start block     ");
    } else if (block_size == 0) {
        printf(" End of region   ");
    } else if (is_free) {
        printf(" Free block      ");
    } else if (!is_free) {
        printf(" Allocated block ");
    } else {
        printf("                 ");
    }
    printf("|\r\n");
}

void
lwmem_debug_print(unsigned char print_alloc, unsigned char print_free) {
    size_t block_size;
    lwmem_block_t* block;


    printf("|-------|--------------|--------|------|------------------|-----------------|\r\n");
    printf("| Block |      Address | IsFree | Size | MaxUserAllocSize | Meta            |\r\n");
    printf("|-------|--------------|--------|------|------------------|-----------------|\r\n");

    block = &lwmem.start_block_first_use;
    print_block(0, &lwmem.start_block_first_use);
    printf("|-------|--------------|--------|------|------------------|-----------------|\r\n");
    for (size_t i = 0, j = 1; i < regions_count; i++) {
        block = regions_orig[i].start_addr;

        /* Print all blocks */
        for (;; j++) {
            block_size = block->size & ~LWMEM_ALLOC_BIT;

            print_block(j, block);

            /* Get next block */
            block = (void *)(LWMEM_TO_BYTE_PTR(block) + block_size);
            if (block_size == 0) {
                break;
            }
        }
        printf("|-------|--------------|--------|------|------------------|-----------------|\r\n");
    }
}

unsigned char
lwmem_debug_create_regions(lwmem_region_t** regs_out, size_t count, size_t size) {
    regions_orig = create_regions(count, size);
    regions_temp = create_regions(count, size);

    if (regions_orig == NULL || regions_temp == NULL) {
        return 0;
    }
    regions_count = count;
    *regs_out = regions_orig;

    return 1;
}

void
lwmem_debug_save_state(void) {
    memcpy(&lwmem_temp, &lwmem, sizeof(lwmem_temp));
    for (size_t i = 0; i < regions_count; i++) {
        memcpy(regions_temp[i].start_addr, regions_orig[i].start_addr, regions_temp[i].size);
    }
    printf(" -- > Current state saved!\r\n");
}

void
lwmem_debug_restore_to_saved(void) {
    memcpy(&lwmem, &lwmem_temp, sizeof(lwmem_temp));
    for (size_t i = 0; i < regions_count; i++) {
        memcpy(regions_orig[i].start_addr, regions_temp[i].start_addr, regions_temp[i].size);
    }
    printf(" -- > State restored to last saved!\r\n");
}

#endif /* defined(LWMEM_DEV) && !__DOXYGEN__ */
