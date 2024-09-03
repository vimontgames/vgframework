# `enumeration_name_v`

## Overview

The `enumeration_name_v` variable template provides a compile-time approach to retrieve the name of an enumeration type as a string literal. It is particularly useful for debugging, logging, or any scenario where the programmer needs to obtain the textual representation of an enumeration type rather than its underlying value.

## Interface Declaration

```cpp

template<concepts::strong_enum enum_type>
inline constexpr std::string_view enumeration_name_v;

```

- **Parameters**:
  - `enum_type`: The enumeration type for which the name is desired. This template parameter should be an enumeration type satisfying a specific enum concept ensuring it's an enum.
  
- **Returns**: 
  - A `std::string_view` holding the name of the enumeration as defined in the code. This is a compile-time constant value.
  
## Usage

To utilize `enumeration_name_v`, ensure that your enumeration type is compatible with the `concepts::strong_enum` used in the template parameter.

## Usage Example

```cpp

#include <simple_enum/simple_enum.hpp>
#include <iostream>

enum class MyEnum { Value1, Value2 };

// Example usage of `enumeration_name_v`
std::cout << simple_enum::enumeration_name_v<MyEnum> << std::endl;  // Output: "MyEnum"

```

This example demonstrates how to print the name of the enumeration `MyEnum` using `enumeration_name_v`. The output will be the name of the enumeration as specified in the code, i.e., "MyEnum". The variable template `enumeration_name_v` simplifies accessing enumeration names without requiring runtime computations, thereby enhancing efficiency and code readability.


