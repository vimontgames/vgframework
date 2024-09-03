# `fmt::format` Support for Enumerations

## Overview

`fmt::format` is a part of the {fmt} library, a modern formatting library for C++. The `simple_enum` library extends `fmt::format` support to enumerations via a custom formatter specialization, enabling easy and efficient string formatting of enum values using the {fmt} library.

## Implementation

To enable `fmt::format` support for your enumeration types, include the custom formatter specialization provided in `simple_enum/fmtlib_format.hpp`.

### Custom Formatter

The `simple_enum/fmtlib_format.hpp` header defines a `fmt::formatter` specialization for enumeration types, leveraging `simple_enum::enum_name` for converting enum values to their string representations.

```cpp
#include <fmt/format.h>

template<simple_enum::enum_concept enumeration>
struct fmt::formatter<enumeration> {
  // Parsing and formatting logic here
};
```

## Usage

Include the `simple_enum/fmtlib_format.hpp` in your project to use `fmt::format` with enumeration types:

```cpp
#include <simple_enum/fmtlib_format.hpp>
```

### Example Code

```cpp
#include <iostream>
#include <simple_enum/fmtlib_format.hpp>  // Include custom formatter support
#include <fmt/format.h>

enum class lorem_ipsum_short { eu, occaecat, dolore };

int main() {
    // Use fmt::format to format an enum value directly
    std::cout << fmt::format("{}\n", lorem_ipsum_short::eu);  // Outputs: eu
}
```

This example demonstrates using `fmt::format` to format an enumeration value directly to a string, provided the enumeration satisfies `simple_enum::enum_concept` and the custom formatter is included.

## Requirements

- The {fmt} library must be included in your project.
- Enumerations should satisfy the `simple_enum::enum_concept`.

Integrating `fmt::format` support for enums enhances the flexibility and expressiveness of formatting operations in C++ applications, making it easier to incorporate enum values into formatted strings with the efficiency and type safety of the {fmt} library.
