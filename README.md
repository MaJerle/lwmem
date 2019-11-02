# Lightweight dynamic memory manager

## Features

- Written in ANSI C99, compatible with `size_t` for size data types
- Implements standard C library functions for memory allocation, `malloc`, `calloc`, `realloc` and `free`
- Uses `first-fit` algorithm to search free block
- Supports different memory regions to allow use of fragmented memories
- Suitable for embedded applications with fragmented memories
- Suitable for automotive applications
- Supports advanced free/realloc algorithms to optimize memory usage
- Operating system ready, thread-safe API
- User friendly MIT license

## Documentation

Full API documentation with description and examples is available and is regulary updated with the source changes

http://majerle.eu/documentation/lwmem/html/index.html

## Contribution

I invite you to give feature request or report a bug. Please use issues tracker.
