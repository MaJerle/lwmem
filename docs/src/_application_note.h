/**
 * \page            page_appnote Application note
 * \tableofcontents
 *
 * \section         sect_getting_started Getting started
 *
 * Repository <a href="https://github.com/MaJerle/lwmem"><b>lwmem is hosted on Github</b></a>. It combines source code and example projects.
 *
 * \subsection      sect_clone Clone repository
 *
 * \par             First-time clone
 *
 *  - Download and install `git` if not already
 *  - Open console and navigate to path in the system to clone repository to. Use command `cd your_path`
 *  - Run `git clone --recurse-submodules https://github.com/MaJerle/lwmem` command to clone repository including submodules
 *  - Navigate to `examples` directory and run favourite example
 *
 * \par             Already cloned, update to latest version
 *
 *  - Open console and navigate to path in the system where your resources repository is. Use command `cd your_path`
 *  - Run `git pull origin master --recurse-submodules` command to pull latest changes and to fetch latest changes from submodules
 *  - Run `git submodule foreach git pull origin master` to update & merge all submodules
 *
 * \section         sect_config Library configuration
 *
 * To make library as efficient as possible, different configuration parameters are available
 * to make sure all the requirements are met for different purposes as possible.
 *
 * A list of all configurations can be found in \ref LWMEM_CONFIG section.
 *
 * \subsection      sect_conf_file Project configuration file
 *
 * Library comes with `2` configuration files:
 *
 *  - Default configuration file `lwmem_config_default.h`
 *  - Project template configuration file `lwmem_config_template.h`
 *
 * When project is started, user has to rename template file to `lwmem_config.h`
 * and if required, it should override default settings in this file.
 *
 * Default template file comes with something like this:
 *
 * \include         _lwmem_config_template.h
 *
 * If bigger buffer is required, modification must be made like following:
 *
 * \include         _lwmem_config.h
 *
 * \note            Always modify default settings by overriding them in user's custom `lwmem_config.h` file
 *                      which was previously renamed from `lwmem_config_template.h`
 *
 * \section         sect_how_it_works How memory allocation works
 *
 * For the sake of example memory manager on images uses `3` regions:
 *
 *  - Region `1` memory starts at `0x1000 0000` and is `0x0000 1000` bytes long
 *  - Region `2` memory starts at `0xA000 0000` and is `0x0000 8000` bytes long
 *  - Region `3` memory starts at `0xC000 0000` and is `0x0000 8000` bytes long
 *  - Total size of memory used by application for memory manager is `0x0001 1000` bytes or `69 kB`
 *
 * Furthermore, example assumes:
 * 
 *  - Size of any kind of pointer is `4-bytes`, `sizeof(any_pointer_type) = 4`
 *  - Size of `size_t` type is `4-bytes`, `sizeof(size_t) = 4`
 *
 * In C code, defining these regions for memory manager would look similar to example below.
 * 
 * \include         example_regions_definitions.c
 *
 * After the initialization process, memory is written with some values, available on picture below.
 *
 * \image html structure_default.svg Default memory structure after initialization
 *
 * Memory managers sets some default values, these are:
 *
 *  - All regions are connected through single linked list. Each member of linked list represents free memory slot
 *  - `Start block` is variable in the library and points to first free memory on the list. By default, this is beginning of first region
 *  - Each region consists of `2 free slots`
 *      - One at the end of each region. It takes `8 bytes` of memory:
 *          - Size of slot is set to `0` and it means no available memory
 *          - Its `next` value points to next free slot in another region. Set to `NULL` if there is no free slot available anymore after or in case of last region on a list
 *      - One on beginning of region. It also takes `8 bytes` of memory:
 *          - Size of slot is set to `region_size - 8`, ignoring size of last slot. 
 *              Effective size of memory application may allocate in region is always for `2` meta slots less than region size,
 *              which means `max_app_malloc_size = region_size - 2 * 8 bytes`
 *          - Its `next` value points to end slot in the same region
 *
 * When application calls one of allocation functions and if requested size of memory is less than maximal one,
 * manager will allocate desired memory and will mark it as used. If requested size is bigger than available memory
 * in first region, manager will continue to check size of next free slots in other regions.
 *
 * \image html structure_first_alloc.svg Memory structure after first allocation
 *
 *  - Light red background slot indicates memory `in use`.
 *  - All blocks marked `in use` have:
 *      - `next` value is set to `NULL`
 *      - `size` value has MSB bit set to `1`, indicating block is allocated and the rest of bits represent size of block, including metadata size
 *      - If application asks for `8 bytes`, fields are written as `next = 0x0000 0000` and `size = 0x8000 000F`
 *  - `Start block` now points to free slot somewhere in the middle of region
 *
 * Follow description of bottom image for more information about allocation and deallocation.
 *
 * \image html structure_alloc_free_steps.svg Step-by-step memory structure after multiple allocations and deallocations
 * 
 * Image shows only first region to simplify process. Same procedure applies to other regions aswell.
 *
 *  - `Case A`: Second block allocated. Remaining memory is now smaller and `Start block` points to it
 *  - `Case B`: Third block allocated. Remaining memory is now smaller and `Start block` points to it
 *  - `Case C`: Forth block allocated. Remaining memory is now smaller and `Start block` points to it
 *  - `Case D`: Third block freed and added back to linked list of free slots.
 *  - `Case E`: Forth block freed. Manager detects blocks before and after current are free and merges all  to one big contiguous block
 *  - `Case F`: First block freed. `Start block` points to it as it has been added back to linked list
 *  - `Case G`: Second block freed. Manager detects blocks before and after current are free and merges all to one big contiguous block.
 *      - No any memory allocated anymore, regions are back to default state
 *
 * \section         sect_realloc_how_it_works Optimized re-allocation algorithm
 *
 * why would you need re-allocation algorithm at first place?
 *
 * Sometimes application uses variable length of memory,
 * specially when number of (for example) elements in not fully known in advance.
 * An example, application anticipates `12` numbers but may (for some reason) in some cases receive more than this.
 * If application needs to hold all received numbers, it may be necessary to:
 *
 *  - Option `1`: Increase memory block size using reallocations on demand
 *  - Option `2`: Use very big (do we know how much?) array, allocated statically or dynamically,
 *         which would hold all numbers at any time possible
 *
 * Application wants to use as less memory as possible, so we will take option `1`, to increase memory only on demand.
 *
 * Let's first define our region(s). For the sake of example, application uses single region:
 *
 * \include         example_realloc_region.c
 *
 * When executed, it prints (test machine)
 *
 * \include         example_realloc_region_log.c
 *
 * Next, application allocates memory for `12 integers`.
 *
 * \include         example_realloc_first_malloc.c
 *
 * When executed, it prints (test machine)
 *
 * \include         example_realloc_first_malloc_log.c
 *
 * We can see, there is one block with `64` bytes available.
 * It means that manager spent `120 - 64 = 56` bytes to allocate memory for `12` integers.
 * 
 * \note            Every allocated block holds meta data.
 *                  On test machine, `sizeof(int) = 4` therefore `8` bytes are used for metadata as `56 - 12 * sizeof(int) = 8`.
 *                  Size of meta data header depends on CPU architecture and may be different on final architecture
 *
 * What happens, if (for some reason) application needs more memory than current block is?
 * Let's say application needs to increase memory to be able to hold `13` integers.
 *
 * The easiest way would be:
 * 
 *  1. Allocate new memory block with new size and check if allocation was successful
 *  2. Manually copy content from old block to new block
 *  3. Free old memory block
 *  4. Use new block for all future operations
 *
 * \include         example_realloc_custom_realloc.c
 *
 * When executed, it prints (test machine)
 * 
 * \include         example_realloc_custom_realloc_log.c
 *
 * Looking at the debug output:
 *
 *  - Memory was successfully allocated for `12` integers, it took `56 bytes`
 *  - Memory was successfully allocated for another `13` integers , it took `64 bytes`
 *  - There is no more free memory available
 *  - First `12` integers array was successfully freed, manager has `56` bytes of free memory
 *  - Second `13` integers block was successfully freed, manager has all `120` bytes available for new allocations
 *
 * What would happen, if application needs to increase block size for `3` more bytes? From `12` to `15`.
 * When same code is executed, but with `15` in second case:
 *
 * \include         example_realloc_custom_realloc_log_2.c
 *
 * It was not possible to allocate new memory for `15` integers. 
 * We just need to increase for `3` more integers, which is `12 bytes`,
 * there are `64` bytes of memory available, but we were not able to use them!
 *
 * The same effect would happen if application wants to decrease integer count from `15` to `12` numbers. 
 * There is not enough memory to allocate new block for `12` integers.
 *
 * \note            Effectively it means that maximal block size we can allocate is less than `50%` of full memory,
 *                  if we intend to increase it in the future using above method.
 *
 * \par             Solution: Try to manipulate existing block as much as possible
 *
 * To avoid having multiple temporary allocations, we could only manipulate existing block to modify its size.
 * Let's start with simplest example.
 *
 * \subsection      sect_realloc_shrinking Shrinking existing block
 *
 * When applications tries to decrease memory size, manager could only modify block size parameters.
 * This will effectively shrink block size and allow other parts of application to
 * allocate memory using memory manager.
 * 
 * \include         example_realloc_shrink.c
 *
 * When executed, it prints (test machine)
 * 
 * \include         example_realloc_shrink_log.c
 *
 * \image html structure_realloc_shrink.svg New size is smaller than existing one
 *
 * Looking at the debug output and image above:
 *
 *  - Memory was successfully allocated for `15` integers, it took `68` bytes; part `A` on image
 *  - Memory was successfully re-allocated to `12` integers, now it takes `56` bytes, part `B` on image
 *  - In both cases on image, final returned memory points to the same address
 *      - Manager does not need to copy data from existing memory to new address as it is the same memory used in both cases
 *  - Empty block start address has been modified and its size has been increased, part `B` on image
 *  - Reallocated block was successfully freed, manager has all `120` bytes for new allocations
 *
 * This was very basic and primitive example. However, it is not always possible to increase free block size.
 * Consider new example and dedicated image below:
 *
 * \include         example_realloc_shrink_fragmented.c
 *
 * \image html structure_realloc_shrink_fragmented.svg Decreasing size on fragmented blocks
 *
 * When executed, it prints (test machine)
 * 
 * \include         example_realloc_shrink_fragmented_log.c
 *
 * Looking at the debug output and image above:
 *
 *  - Size of all `4` blocks is `24` bytes; `16` for user data, `8` for metadata
 *  - Reallocating block first time from `16` to `12` user data bytes did not affect internal memory structure
 *      - It is not possible to create new empty block as it would be too small, only `4` bytes available, minimum is `8` bytes
 *      - It is not possible to enlarge next empty block as current and next empty do not create contiguous block
 *      - Block is internally left unchanged
 *  - Reallocating block second time to `8` bytes was successful
 *      - Difference between old and new size is `8` bytes which is enough for new empty block.
 *           Its size is `8` bytes, effectively `0` for user data
 *
 * \par             Summary for reallocation to smaller size
 *
 * When reallocating already allocated memory block, one of `3` cases will happen:
 *
 *  - Case `1`: When current block and next free block could create contigouos block of memory, 
 *       current block is decreased (size parameter) and next free is enlarged by the size difference
 *  - Case `2`: When difference between current size and new size is more or equal to minimal size for new empty block,
 *       new empty block is created with size `current_size - new_size` and added to list of free blocks
 *  - Case `3`: When difference between current size and new size is less than minimal size for new empty block,
 *       block is left unchanged
 *
 * \subsection      sect_realloc_enlarging Enlarging existing block
 *
 * Things get more tricky when we need to enlarge existing block.
 * Since we are increasing block size, we cannot just anticipate memory after already allocated block is free.
 * We need to take some actions and optimize reallocation procedure.
 *
 * Possible memory structure cases are explained below.
 *
 * \note            Size numbers represent block size, including meta size.
 *                  Number `32` represents `24` user bytes and `8` metadata bytes.
 *
 * \par             Free block before allocated [block] create one big contiguous block
 *
 * \image html structure_realloc_enlarge_1.svg Case 1: Free block before allocated [block] create one big contiguous block
 *
 * This is the simplest way to show how reallocation works. Let's start by code and output:
 *
 * \include         example_realloc_enlarge_1.c
 *
 * When executed, it prints (test machine)
 * 
 * \include         example_realloc_enlarge_1_log.c
 *
 * Looking at the debug output and image above:
 *
 *  - Allocation for first block of memory (`24` user bytes) uses `32` bytes of data
 *  - Reallocation is successful, block has been extended to `40` bytes and next free shrinked down to `80` bytes
 *
 * \par             Free block after allocated [block] create one big contiguous block
 *
 * \image html structure_realloc_enlarge_2.svg Case 2: Free block after allocated [block] create one big contiguous block
 *
 * Second case is a block with free block as previous.
 * As you can see on image, it is possible to reallocate existing block by moving it to the left.
 * Example code which would effectively produce this:
 *
 * \include         example_realloc_enlarge_2.c
 *
 * When executed, it prints (test machine)
 * 
 * \include         example_realloc_enlarge_2_log.c
 *
 * Looking at the debug output and image above:
 *
 *  - First we allocate big block (`88` bytes), followed by smaller block (`32` bytes)
 *  - We then free big block to mark it as free. This is effectively state `2a`
 *  - During reallocation, manager did not find suitable block after, but did find suitable block before current one:
 *      - Empty block and allocated block are temporary merged to one big block (`120` bytes)
 *      - Content of allocated block is shifted up to beginning of big block
 *      - Big block is later splitted to required size, the rest is marked as free
 *  - This is effectively state `2b`
 *
 * \par             Free block before and after allocated [block] create one big contiguous block
 *
 * \image html structure_realloc_enlarge_3.svg Case 2: Free block before and after allocated [block] create one big contiguous block
 * 
 * In this example we always have `2` allocated blocks and we want to reallocate `green` block. 
 * `Red` block is there acting as an obstacle to show different application use cases.
 *
 * Image shows `4` optional cases. For every case, case labeled with `3` is initial state.
 *
 * \note            Case labelled with `3` is initial state for any of `3a - 3d` cases
 *
 * Before moving to the description of the cases, let's make initial state with C code example
 *
 * \include         example_realloc_enlarge_3.c
 *
 * When executed, it prints (test machine)
 * 
 * \include         example_realloc_enlarge_3_log.c
 *
 * As seen on the image (and confirmed in log), there are `3` free slots of `16, 12 and 56` bytes respectively.
 *
 * Cases:
 *  - Case `3a`: Application tries to reallocate green block from `12` to `16` bytes
 *      - Reallocation is successful, there is a free block just after and green block is successfully enlarged
 *      - Block after is shrinked from `12` to `8` bytes
 *      - Code example (follows initial state code example)
 *            \include  example_realloc_enlarge_3a.c
 *      - When executed, it prints (test machine)
 *            \include  example_realloc_enlarge_3a_log.c
 *  - Case `3b`: Application tries to reallocate green block from `12` to `28` bytes 
 *      - Block after green is not big enough to merge them to one block (`12 + 12 < 28`)
 *      - Block before green is big enough (`16 + 12 >= 28`)
 *      - Green block is merged with previous free block and content is shifted to the beginning of new block
 *      - Code example (follows initial state code example)
 *            \include  example_realloc_enlarge_3b.c
 *      - When executed, it prints (test machine)
 *            \include  example_realloc_enlarge_3b_log.c
 *  - Case `3c`: Application tries to reallocate green block from `12` to `32` bytes
 *      - Block after green is not big enough to merge them to one block (`12 + 12 < 32`)
 *      - Block before green is also not big enough (`12 + 16 < 32`)
 *      - All three blocks together are big enough (`16 + 12 + 12 >= 32`)
 *      - All blocks are effectively merged together and there is a new temporary block with its size set to `40` bytes
 *      - Content of green block is shifted to the beginning of new block
 *      - New block is limited to `32` bytes, keeping `8` bytes marked as free at the end
 *      - Code example (follows initial state code example)
 *            \include  example_realloc_enlarge_3c.c
 *      - When executed, it prints (test machine)
 *            \include  example_realloc_enlarge_3c_log.c
 *  - Case `3d`: Application tries to reallocate green block from `12` to `44` bytes
 *      - None of the methods (`3a - 3c`) are available as blocks are too small
 *      - Completely new block is created and content is copied to it
 *      - Existing block is marked as free. Since all `3` free blocks create big contiguous block,
 *          we can merge them to one block with its size set to `40`
 *      - Code example (follows initial state code example)
 *            \include  example_realloc_enlarge_3d.c
 *      - When executed, it prints (test machine)
 *            \include  example_realloc_enlarge_3d_log.c
 *
 * \par             Full code example with new debug output
 *
 * Advanced debugging features has been added for development purposes.
 * It is now possible to simulate different cases within single executable,
 * by storing states to different memories.
 *
 * Example has been implemented which runs on WIN32 and 
 * relies on dynamic allocation using `malloc` standard C function,
 * to prepare blocks of memory later used by lwmem
 *
 * How it works:
 *  - Code prepares state `3` and saves memory to temporary memory for future restore
 *  - Code restores latest saved state (case `3`) and executes case `3a`
 *  - Code restores latest saved state (case `3`) and executes case `3b`
 *  - Code restores latest saved state (case `3`) and executes case `3c`
 *  - Code restores latest saved state (case `3`) and executes case `3d`
 *
 * Full code example 
 * \include         example_realloc_enlarge_full.c
 *
 * When executed, it prints (test machine)
 * \include         example_realloc_enlarge_full_log.c
 *
 * \section         sect_thread_safety Concurrent access & thread safety
 *
 * Many embedded applications run operating system and this is where thread safety is important.
 * LwMEM by default uses single resource for all allocations, meaning that during memory operation (allocation, freeing) 
 * different threads MUST NEVER interrupt current running thread, or memory structure (and system) may collapse.
 *
 * \note            Same concern applies when allocating/freeing memory from main thread and interrupt context,
 *                  when not using operating system. It is advised NOT to do any LwMEM operation in interrupt context
 *                  or memory structure (and system) may collapse.
 *
 * When using LwMEM in operating system environment, it is possible to use it with system protection functions.
 * One more layer of functions needs to be implemented.
 *
 * Function description which needs to be implemented and added to project are available in \ref LWMEM_SYS section.
 * Example codes are available in official git repository under `system` folder.
 *
 */