# Lightweight dynamic memory manager

<h3>Read first: <a href="http://docs.majerle.eu/projects/lwmem/">Documentation</a></h3>

## Features

* Written in C (C11), compatible with ``size_t`` for size data types
* Implements standard C library functions for memory allocation, malloc, calloc, realloc and free
* Uses *first-fit* algorithm to search for free block
* Supports multiple allocation instances to split between memories and/or CPU cores
* Supports different memory regions to allow use of fragmented memories
* Highly configurable for memory allocation and reallocation
* Supports embedded applications with fragmented memories
* Supports automotive applications
* Supports advanced free/realloc algorithms to optimize memory usage
* **Since v2.2.0** Supports light implementation with allocation only
* Operating system ready, thread-safe API
* C++ wrapper functions
* User friendly MIT license

## Contribute

Fresh contributions are always welcome. Simple instructions to proceed:

1. Fork Github repository
2. Follow [C style & coding rules](https://github.com/MaJerle/c-code-style) already used in the project
3. Create a pull request to develop branch with new features or bug fixes

Alternatively you may:

1. Report a bug
2. Ask for a feature request
