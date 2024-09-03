# Compiler and Standard Support for simple_enum

## Overview

`simple_enum` is designed with modern C++ practices in mind, emphasizing efficiency and ease of use for compile-time enumeration handling. This document outlines the compiler support and C++ language standards compatibility for `simple_enum`.

## Compiler Support

`simple_enum` is actively developed and tested with a focus on compatibility with the following compilers:

- **Clang**: Actively developed on Clang 17 with llvm libc++ nad glibc++, with support extending down to Clang 13.
- **GCC**: Developed using GCC 13, while ensuring backward compatibility with versions as early as GCC 10.
- **MSVC**: Tested with CI to ensure compatibility with Microsoft Visual Studio's compiler, MSVC.


## C++ Language Standard

`simple_enum` requires a minimum of the **C++20 standard** for its core functionality. This requirement allows for the use of recent language features that facilitate more efficient and readable code.

Furthermore, where possible, `simple_enum` is tested with and adopts features from the **C++23 standard**, staying ahead with the latest enhancements in the C++ language. This adoption ensures that `simple_enum` can provide the most advanced functionality and performance optimizations available.

## Future Compatibility

The development team is committed to maintaining compatibility with upcoming C++ standards and compiler versions. As new standards are finalized and compiler support for these standards evolves, `simple_enum` will be updated to utilize new language features that can enhance its functionality and performance.

## Getting Help

If you encounter any issues related to compiler compatibility or have suggestions for improving `simple_enum`'s support for various C++ standards, please [open an issue](https://github.com/arturbac/simple_enum/issues) on our GitHub repository. Community feedback is invaluable in making `simple_enum` more robust and versatile.

