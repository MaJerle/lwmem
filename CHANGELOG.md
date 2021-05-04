# Changelog

## Develop

- Fix missing region parameter in some allocation or reallocation cases

## v1.5.1

- Fix memory cleanup macro setup

## v1.5.0

- Add option to cleanup memory on free and realloc operations

## v1.4.0

- New artistic code style
- Replace configuration
- Several bug fixes

## v1.3.0

- Added option for custom LwMEM instance for complete isolation
- Added `_ex` functions for extended features for custom instances
- Added macros for backward-compatible, default instance, allocation functions
- Added option to force LwMEM to allocate memory at specific region
- Added win32 examples

## v1.2.0

- Fix comments to easily undestand architecture
- Migrate examples to CMSIS-OS v2
- Use pre-increment instead of post-increment
- Other C code style fixes

## v1.1.0

- Added support for thread-safety application
- Added boundary check of all internal pointers
- Added tests code directly in library
- Bug fixes
