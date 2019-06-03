/**	
 * \file            lwmem.c
 * \brief           Lightweight dynamic memory manager
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
 * This file is part of Lightweight dynamic memory manager library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 */
#include "lwmem/lwmem.h"
#include "limits.h"

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
 * \brief           Set block as allocated
 * \param[in]       block: Block to set as allocated
 */
#define LWMEM_BLOCK_SET_ALLOC(block)    do { if (NULL != (block)) { (block)->size |= LWMEM_ALLOC_BIT; (block)->next = (void *)(LWMEM_TO_BYTE_PTR(0) + 0xDEADBEEF); }} while (0)

/**
 * \brief           Check if input block is properly allocated and valid
 * \param[in]       block: Block to check if properly set as allocated
 */
#define LWMEM_BLOCK_IS_ALLOC(block)     (NULL != (block) && ((block)->size & LWMEM_ALLOC_BIT) && (void *)(LWMEM_TO_BYTE_PTR(0) + 0xDEADBEEF) == (block)->next)

/**
 * \brief           Bit indicating memory block is allocated
 */
#define LWMEM_ALLOC_BIT                 ((size_t)((size_t)1 << (sizeof(size_t) * CHAR_BIT - 1)))

/**
 * \brief           Get block handle from application pointer
 * \param[in]       ptr: Input pointer to get block from
 */
#define LWMEM_GET_BLOCK_FROM_PTR(ptr)   (void *)(NULL != (ptr) ? ((LWMEM_TO_BYTE_PTR(ptr)) - LWMEM_BLOCK_META_SIZE) : NULL)

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
    for ((_pp_) = NULL, (_p_) = &start_block;                           \
        NULL != (_p_) && (_p_)->next < (_b_);                           \
        (_pp_) = (_p_), (_p_) = (_p_)->next                             \
    ) {}                                                                \
} while (0)

/**
 * \brief           Memory block structure
 */
typedef struct lwmem_block {
    struct lwmem_block* next;                   /*!< Next free memory block on linked list.
                                                        Set to `NULL` when block is allocated and in use */
    size_t size;                                /*!< Size of block. MSB bit is set to `1` when block is allocated and in use,
                                                        or `0` when block is free */
} lwmem_block_t;

static lwmem_block_t start_block;               /*!< Holds beginning of memory allocation regions */
static lwmem_block_t* end_block;                /*!< Pointer to the last memory location in regions linked list */
static size_t mem_available_bytes;              /*!< Memory size available for allocation */
static size_t mem_regions_count;                /*!< Number of regions used for allocation */

/**
 * \brief           Insert free block to linked list of free blocks
 * \param[in]       nb: New free block to insert into linked list
 */
static void
prv_insert_free_block(lwmem_block_t* nb) {
    lwmem_block_t* prev;

    /* 
     * Try to find position to put new block
     * Search until all free block addresses are lower than new block
     */
    for (prev = &start_block; NULL != prev && prev->next < nb; prev = prev->next) {}

    /* This is hard error with wrong memory usage */
    if (NULL == prev) {
        return;
    }

    /*
     * At this point we have valid previous block
     * Previous block is free block before new block
     */

    /*
     * Check if previous block and new block together create one big contiguous block
     * If this is the case, merge blocks together and increase previous block by new block size
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
    if (NULL != prev->next && prev->next->size  /* Do not remove "end of region" indicator in each region */
        && (LWMEM_TO_BYTE_PTR(nb) + nb->size) == LWMEM_TO_BYTE_PTR(prev->next)) {
        if (prev->next == end_block) {          /* Does it points to the end? */
            nb->next = end_block;               /* Set end block pointer */
        } else {
            nb->size += prev->next->size;       /* Expand of current block for size of next free block which is right behind new block */
            nb->next = prev->next->next;        /* Next free is pointed to the next one of previous next */
        }
    } else {
        nb->next = prev->next;                  /* Set next of new block as next of current one */
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
 * \param[in]       block_size: Final block size
 * \param[in]       set_as_alloc: Set to `1` to set input block as allocated, `0` otherwise
 * \return          `1` if block splitted, `0` otherwise
 */
static unsigned char
prv_split_too_big_block(lwmem_block_t* block, size_t block_size, unsigned char set_as_alloc) {
    lwmem_block_t* next;
    unsigned char success = 0;

    /*
     * If current block size is greater than requested size,
     * it is possible to create empty block at the end of existing one
     * and add it back to list of empty blocks
     */

    if ((block->size - block_size) >= LWMEM_BLOCK_MIN_SIZE) {
        next = (void *)(LWMEM_TO_BYTE_PTR(block) + block_size); /* Put next block after size of current allocation */
        next->size = block->size - block_size;  /* Modify block data */
        block->size = block_size;               /* Current size is now smaller */

        mem_available_bytes += next->size;      /* Increase available bytes by new block size */
        prv_insert_free_block(next);            /* Add new block to the free list */

        success = 1;
    } else {
        /* TODO: If next of current is free, check if we can increase next by at least some bytes */
        /* This can only happen during reallocation process when allocated block is reallocated to previous one */
        /* Very rare case, but may happen! */
    }
    if (set_as_alloc) {
        LWMEM_BLOCK_SET_ALLOC(block);           /* Set as allocated */
    }
    return success;
}

/**
 * \brief           Get application memory size of allocated pointer
 * \param[in]       ptr: Allocated pointer
 * \return          Application block memory size in units of bytes
 */
static size_t
block_app_size(void* const ptr) {
    lwmem_block_t* const block = LWMEM_GET_BLOCK_FROM_PTR(ptr); /* Get meta from application address */;
    if (LWMEM_BLOCK_IS_ALLOC(block)) {          /* Check if block is valid */
        return (block->size & ~LWMEM_ALLOC_BIT) - LWMEM_BLOCK_META_SIZE;
    }
    return 0;
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
    if (NULL == end_block || LWMEM_BLOCK_META_SIZE == final_size || (final_size & LWMEM_ALLOC_BIT)) {
        return NULL;
    }

    /* Try to find first block with has at least `size` bytes available memory */
    prev = &start_block;                        /* Always start with start block which contains valid information about first available block */
    curr = prev->next;                          /* Set current as next of start = first available block */
    while (curr->size < final_size) {           /* Loop until available block contains less memory than required */
        if (NULL == curr->next || curr == end_block) {  /* If no more blocks available */
            return NULL;                        /* No sufficient memory available to allocate block of memory */
        }
        prev = curr;                            /* Set current as previous */
        curr = curr->next;                      /* Go to next empty entry */
    }

    /* There is a valid block available */
    retval = (void *)(LWMEM_TO_BYTE_PTR(prev->next) + LWMEM_BLOCK_META_SIZE);   /* Return pointer does not include meta part */
    prev->next = curr->next;                    /* Remove this block from linked list by setting next of previous to next of current */

    /* curr block is now removed from linked list */
    
    /* 
     * If block size is bigger than required,
     * split it to to make available memory for other allocations
     * First check if there is enough memory for next free block entry
     */
    mem_available_bytes -= curr->size;          /* Decrease available bytes by allocated block size */
    prv_split_too_big_block(curr, final_size, 1);   /* Split block if necessary and set it as allocated */

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

        mem_available_bytes += block->size;     /* Increase available bytes */
        prv_insert_free_block(block);           /* Put block back to list of free block */
    }
}

/**
 * \brief           Initialize and assigns user regions for memory used by allocator algorithm
 * \param[in]       regions: Array of regions with address and its size.
 *                      Regions must be in increasing order (start address) and must not overlap in-between
 * \param[in]       len: Number of regions in array
 * \return          `0` on failure, number of final regions used for memory manager on success
 */
size_t
LWMEM_PREF(assignmem)(const LWMEM_PREF(region_t)* regions, const size_t len) {
    unsigned char* mem_start_addr;
    size_t mem_size;
    lwmem_block_t* first_block, *prev_end_block;

    if (NULL != end_block                       /* Init function may only be called once */
        || (LWMEM_ALIGN_NUM & (LWMEM_ALIGN_NUM - 1))) { /* Must be power of 2 */
        return 0;
    }

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
         * If NULL == end_block, this indicates first iteration.
         * In first indication application shall set start_block and never again
         * end_block value holds
         */
        if (NULL == end_block) {
            /*
             * Next entry of start block is first region
             * It points to beginning of region data
             * In the later step(s) first block is manually set on top of memory region
             */
            start_block.next = (void *)mem_start_addr;
            start_block.size = 0;               /* Size of dummy start block is zero */
        }

        /* Save current end block status as it is used later for linked list insertion */
        prev_end_block = end_block;

        /* Put end block to the end of the region with size = 0 */
        end_block = (void *)(mem_start_addr + mem_size - LWMEM_BLOCK_META_SIZE);
        end_block->next = NULL;                 /* End block in region does not have next entry */
        end_block->size = 0;                    /* Size of end block is zero */

        /*
         * Create memory region first block.
         *
         * First block meta size includes size of metadata too
         * Subtract MEM_BLOCK_META_SIZE as there is one more block (end_block) at the end of region
         *
         * Actual maximal available size for application in the region is mem_size - 2 * MEM_BLOCK_META_SIZE
         */
        first_block = (void *)mem_start_addr;
        first_block->next = end_block;          /* Next block of first is last block */
        first_block->size = mem_size - LWMEM_BLOCK_META_SIZE;

        /* Check if previous regions exist by checking previous end block state */
        if (NULL != prev_end_block) {
            prev_end_block->next = first_block; /* End block of previous region now points to start of current region */
        }

        mem_available_bytes += first_block->size;   /* Increase number of available bytes */
        mem_regions_count++;                    /* Increase number of used regions */
    }

    return mem_regions_count;                   /* Return number of regions used by manager */
}

/**
 * \brief           Allocate memory of requested size
 * \note            Function declaration is in-line with standard C function `malloc`
 * \param[in]       size: Number of bytes to allocate
 * \return          Pointer to allocated memory on success, `NULL` otherwise
 */
void *
LWMEM_PREF(malloc)(const size_t size) {
    return prv_alloc(size);
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
 */
void *
LWMEM_PREF(calloc)(const size_t nitems, const size_t size) {
    void* ptr;
    const size_t s = size * nitems;

    if (NULL != (ptr = prv_alloc(s))) {
        LWMEM_MEMSET(ptr, 0x00, s);
    }
    return ptr;
}

/**
 * \brief           Reallocates already allocated memory with new size
 *
 * Function behaves differently, depends on input parameter of `ptr` and `size`:
 *
 *  - `NULL == ptr; !size`: Function returns `NULL`, no memory is allocated or freed
 *  - `NULL == ptr; size`: Function tries to allocate new block of memory with `size` length, equivalent to `malloc(size)`
 *  - `NULL != ptr; !size`: Function frees memory, equivalent to `free(ptr)`
 *  - `NULL != ptr; size`: Function tries to allocate new memory of copy content before returning pointer on success
 *
 * \note            Function declaration is in-line with standard C function `realloc`
 *
 * \param[in]       ptr: Memory block previously allocated with one of allocation functions.
 *                      It may be set to `NULL` to create new clean allocation
 * \param[in]       size: Size of new memory to reallocate
 * \return          Pointer to allocated memory on success, `NULL` otherwise
 */
void *
LWMEM_PREF(realloc)(void* const ptr, const size_t size) {
    lwmem_block_t* block, *prevprev, *prev;
    size_t block_size;
    void* retval;

    /* Calculate final size including meta data size */
    const size_t final_size = LWMEM_ALIGN(size) + LWMEM_BLOCK_META_SIZE;

    /* Check optional input parameters */
    if (!size) {
        if (NULL != ptr) {
            LWMEM_PREF(free)(ptr);
        }
        return NULL;
    }
    if (NULL == ptr) {
        return prv_alloc(size);
    }

    /* Try to reallocate existing pointer */
    if ((size & LWMEM_ALLOC_BIT) || (final_size & LWMEM_ALLOC_BIT)) {
        return NULL;
    }

    /* Process existing block */
    retval = NULL;
    block = LWMEM_GET_BLOCK_FROM_PTR(ptr);
    if (LWMEM_BLOCK_IS_ALLOC(block)) {
        block_size = block->size & ~LWMEM_ALLOC_BIT;/* Get actual block size, without memory allocation bit */

        /* If sizes are the same? */
        if (block_size == final_size) {
            return ptr;                         /* Just return pointer, nothing to do */
        }

        /*
         * When new requested size is smaller than existing one,
         * it is enough to modify size of current block only.
         *
         * If new requested size is much smaller than existing one,
         * check if it is possible to create new empty block and add it to list of empty blocks
         *
         * Application returns same pointer back to user.
         */
        if (final_size < block_size) {
            if ((block_size - final_size) >= LWMEM_BLOCK_MIN_SIZE) {
                block->size &= LWMEM_ALLOC_BIT; /* Temporarly remove allocated bit */
                prv_split_too_big_block(block, final_size, 0);  /* Split block if necessary */
            } else {
                /*
                 * It is not possible to create new empty block as it is not enough memory
                 * available at the end of current block
                 * 
                 * But if block just after current one is free, 
                 * we could shift it up and increase its size by "block_size - final_size" bytes
                 */

                /* Find free blocks before input block */
                LWMEM_GET_PREV_CURR_OF_BLOCK(block, prevprev, prev);

                /* Check if current block and next free are connected */
                if ((LWMEM_TO_BYTE_PTR(block) + block_size) == LWMEM_TO_BYTE_PTR(prev->next)
                    && prev->next->size > 0) {  /* Must not be end of region indicator */
                    const size_t tmp_size = prev->next->size;
                    void* const tmp_next = prev->next->next;

                    /* Shift block up, effectively increasing block */
                    prev->next = (void *)(LWMEM_TO_BYTE_PTR(prev->next) - (block_size - final_size));
                    prev->next->size = tmp_size + (block_size - final_size);
                    prev->next->next = tmp_next;
                    mem_available_bytes += block_size - final_size; /* Increase available bytes by new block size */

                    block->size = final_size;   /* Block size is requested size */
                }
            }
            LWMEM_BLOCK_SET_ALLOC(block);       /* Set block as allocated */
            
            return ptr;                         /* Return existing pointer */
        }

        /* New requested size is bigger than current block size is */

        /* Find "curr" free block, located before input existing block */
        LWMEM_GET_PREV_CURR_OF_BLOCK(block, prevprev, prev);
		
		/* Order of variables is: | prevprev ---> prev --->--->--->--->--->--->--->--->--->---> prev->next  | */
		/*                        |                      (input_block, which is not on a list)              | */
        /* Input block points to address somewhere between "prev" and "prev->next" pointers                   */

        /* Check if "block" and "next" create contiguous memory with size of at least new requested size */
        if ((LWMEM_TO_BYTE_PTR(block) + block_size) == LWMEM_TO_BYTE_PTR(prev->next)) {
            /* 
             * 2 blocks create contiguous memory
             * Is size of 2 blocks together big enough?
             */
            if ((block_size + prev->next->size) >= final_size) {
                /* Merge blocks together by increasing its size and removing it from free list */
                mem_available_bytes -= prev->next->size;/* For now decrease effective available bytes */
                block->size = block_size + prev->next->size;/* Increase effective size of new block */
                prev->next = prev->next->next;  /* Set next to next's next, effectively remove expanded block from free list */

                prv_split_too_big_block(block, final_size, 1);  /* Split block if necessary and set it as allocated */
                return ptr;                     /* Return existing pointer */
            }
        }

        /* 
         * Check if current block and one before create contiguous memory
         * In this case, memory move is requested to shift content up in the address space
         * Feature not implemented as not 100% necessary
         */
        if ((LWMEM_TO_BYTE_PTR(prev) + prev->size) == LWMEM_TO_BYTE_PTR(block)) {
            /* 
             * 2 blocks create contiguous memory
             * Is size of 2 blocks together big enough for requested size?
             */
            if ((prev->size + block_size) >= final_size) {
                /* Move memory from block to block previous to current */
                void* const old_data_ptr = (LWMEM_TO_BYTE_PTR(block) + LWMEM_BLOCK_META_SIZE);
                void* const new_data_ptr = (LWMEM_TO_BYTE_PTR(prev) + LWMEM_BLOCK_META_SIZE);
                LWMEM_MEMMOVE(new_data_ptr, old_data_ptr, block_size);  /* Copy old buffer size to new location */

                /*
                 * If memmove overwrites metadata of current block, it is not an issue
                 * as we know its size and next is already NULL
                 */

                mem_available_bytes -= prev->size;  /* For now decrease effective available bytes */
                prev->size += block_size;       /* Increase size of free block size */
                prevprev->next = prev->next;    /* Remove curr from free list as it is now being used for allocation together with existing block */
                block = prev;                   /* Block is now current */

                prv_split_too_big_block(block, final_size, 1);  /* Split block if necessary and set it as allocated */
                return new_data_ptr;            /* Return new data ptr */
            }
        }

        /*
         * Tt was not possible to expand current block with either previous one or next one only.
         * Last option is to check if both blocks (before and after) around current one
         * are free and if all together create block big enough for allocation
         */
        if ((LWMEM_TO_BYTE_PTR(prev) + prev->size) == LWMEM_TO_BYTE_PTR(block)  /* Input block and free block before create contiguous block? */
            && (LWMEM_TO_BYTE_PTR(block) + block_size) == LWMEM_TO_BYTE_PTR(prev->next)) {  /* Input block and free block after create contiguous block? */
            /*
             * Current situation is:
             * 
             * prev block + block + next block make one big contiguous block of size:
             * 
             * Free block before + current input + free block after
             */
            if ((prev->size + block_size + prev->next->size) >= final_size) {
                /* Move memory from block to block previous to current */
                void* const old_data_ptr = (LWMEM_TO_BYTE_PTR(block) + LWMEM_BLOCK_META_SIZE);
                void* const new_data_ptr = (LWMEM_TO_BYTE_PTR(prev) + LWMEM_BLOCK_META_SIZE);
                LWMEM_MEMMOVE(new_data_ptr, old_data_ptr, block_size);  /* Copy old buffer size to new location */

                /*
                 * If memmove overwrites metadata of current block, it is not an issue
                 * as we know its size and next is already NULL
                 *
                 * memmove cannot overwrite free block after current one as we are shifting data up, 
                 * so there is no need to save temporary variables
                 */

                mem_available_bytes -= prev->size + prev->next->size;   /* Decrease effective available bytes for free blocks before and after current one */
                prev->size += block_size + prev->next->size;    /* Increase size of new block */
                prevprev->next = prev->next->next;  /* Remove free block before current one and block after current one from linked list */
                block = prev;                   /* Previous block is now current */

                prv_split_too_big_block(block, final_size, 1);  /* Split block if necessary and set it as allocated */
                return new_data_ptr;            /* Return new data ptr */
            }

            /*
             * It was not possible to do any combination of blocks
             * Next step is to manually allocate new block and copy data
             */
        }
    }

    /*
     * At this stage, it was not possible to modify existing block in any possible way
     * Some manual work is required by allocating new memory and copy content to it
     */
    retval = prv_alloc(size);                   /* Try to allocate new block */
    if (NULL != retval) {
        block_size = block_app_size(ptr);       /* Get application size from input pointer */
        LWMEM_MEMCPY(retval, ptr, size > block_size ? block_size : size);
        prv_free(ptr);                          /* Free previous pointer */
    }
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
 *  - `NULL == ptr`: Invalid input, function returns `0`
 *  - `NULL == *ptr; !size`: Function returns `0`, no memory is allocated or freed
 *  - `NULL == *ptr; size`: Function tries to allocate new block of memory with `size` length, equivalent to `malloc(size)`
 *  - `NULL != *ptr; !size`: Function frees memory, equivalent to `free(ptr)`, sets input pointer pointing to `NULL`
 *  - `NULL != *ptr; size`: Function tries to allocate new memory of copy content before returning pointer on success
 *
 * \param[in]       ptr: Pointer to pointer to allocated memory. Must not be set to `NULL`.
 *                      If reallocation is successful, it modified where pointer points to,
 *                      or sets it to `NULL` in case of `free` operation
 * \param[in]       size: New requested size
 * \return          `1` if successfully reallocated, `0` otherwise
 */
unsigned char
LWMEM_PREF(realloc_s)(void** const ptr, const size_t size) {
    void* new_ptr;

    /*
     * Input pointer must not be NULL otherwise,
     * in case of successful allocation, we have memory leakage
     * aka. allocated memory where noone is pointing to it
     */
    if (NULL == ptr) {
        return 0;
    }
    
    new_ptr = LWMEM_PREF(realloc)(*ptr, size);  /* Try to reallocate existing pointer */
    if (NULL != new_ptr) {
        *ptr = new_ptr;
    } else if (!size) {                         /* !size means free input memory */
        *ptr = NULL;
        return 1;
    }
    return NULL != new_ptr;
}

/**
 * \brief           Free previously allocated memory using one of allocation functions
 * \note            Function declaration is in-line with standard C function `free`
 * \param[in]       ptr: Memory to free. `NULL` pointer is valid input
 */
void
LWMEM_PREF(free)(void* const ptr) {
    prv_free(ptr);                              /* Free pointer */
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
 */
void
LWMEM_PREF(free_s)(void** const ptr) {
    if (NULL != ptr) {
        if (NULL != *ptr) {
            LWMEM_PREF(free)(*ptr);
        }
        *ptr = NULL;
    }
}
