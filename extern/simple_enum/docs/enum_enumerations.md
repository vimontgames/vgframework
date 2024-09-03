# `enum_enumerations` View over all values of a bounded enumeration

## Overview

The `enum_enumerations` function object provides a convenient way to obtain a view over all values of a bounded enumeration. This is particularly useful for enumerations where a specific range of valid values is defined through the use of `first` and `last` markers. By leveraging the `enum_view` abstraction, it enables efficient range-based iteration and application of range algorithms on the set of enumeration values.

## Features

- Specifically designed for **bounded enumerations** that have explicitly defined ranges.
- Utilizes `enum_view` for creating a view that facilitates easy iteration over enumeration values.
- Supports range-based operations, enhancing the usability of enumeration types in algorithms and loops.

## Usage Example

The following example demonstrates how to use `enum_enumerations` to iterate over the values of a bounded enumeration `Color`, printing out the integer representation of each color:

```cpp
enum class Color { Red, Green, Blue, first = Red, last = Blue };

auto color_values = enum_enumerations<Color>();
for (auto enum_value : color_values) {
    std::cout << int(enum_value) << std::endl; // Outputs the integer value of each enumeration member
}
```
 
