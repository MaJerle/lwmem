/**
 * \mainpage
 * \tableofcontents
 * 
 * LwMEM is a dynamic memory manager which implements standard C allocation functions (`malloc`, `realloc`, `calloc`, `free`) and is platform independant.
 *
 * \section         sect_features Features
 *
 *  - Written in ANSI C99, compatible with `size_t` for size data types
 *  - Implements standard C library functions for memory allocation, `malloc`, `calloc`, `realloc` and `free`
 *  - Supports different memory regions to allow use of framented memories
 *  - Uses `first-fit` algorithm to search free block
 *  - Implements optimized reallocation algorithm to find best block
 *  - Suitable for embedded applications with fragmented memories
 *  - Suitable for automotive applications
 *  - Operating system ready
 *  - 100% open source, code available
 *  - User friendly MIT license
 *
 * \section         sect_resources Download & Resources
 *
 *  - <a class="download_url" href="https://github.com/MaJerle/lwmem/releases">Download library from Github releases</a>
 *  - <a href="https://github.com/MaJerle/lwmem_res">Resources and examples repository</a>
 *  - Read \ref page_appnote before you start development
 *  - <a href="https://github.com/MaJerle/lwmem">Official development repository on Github</a>
 *
 * \section         sect_contribute How to contribute
 * 
 *  - Official development repository is hosted on Github
 *  - <a href="https://github.com/MaJerle/c_code_style">Respect C style and coding rules</a>
 *
 * \section         sect_license License
 *
 * \verbatim        
 * Copyright (c) 2019 Tilen Majerle
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
 * OTHER DEALINGS IN THE SOFTWARE. \endverbatim
 *
 */