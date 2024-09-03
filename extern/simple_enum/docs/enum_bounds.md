#  Three Ways of Definig Enum Bounds

- [ADL specified bounds using function `adl_enum_bounds`](#adl-specified-bounds-using-function-adl_enum_bounds)
- [`simple_enum::info` Template struct specialization](#info-struct-specialization)
- [Defining enumeration bounds in-class](#defining-enumeration-bounds-in-class)
- [Full example](#full-example)

# Priority order of bounds definition evaluation in `simple_enum`

- ADL `adl_enum_bounds` declaration
- `simple_enum::info` template specializaiton 
- in class definition of first, last

# (ADL) specified bounds using function `adl_enum_bounds`

## Overview

`adl_enum_bounds` is a user-definable function leveraging Argument-Dependent Lookup (ADL) to specify custom bounds for enumerations.

## Interface Declaration

While `adl_enum_bounds` itself does not have a direct interface in the traditional sense, it relies on user function overloads to provide bounds:

```cpp
#include <simple_enum/core.hpp>

constexpr auto adl_enum_bounds(enumeration) -> simple_enum::adl_info<enumeration>;
```

- **Expected Return**: An instance of `adl_info<enumeration>` constructed with the first and last bounds of the enumeration. enumeration function argument should be ignored and used only to enable (ADL) lookup.

## Template Deduction Guide

To assist in the construction of `adl_info` instances, a template deduction guide is provided:

```cpp
template<typename enumeration>
adl_info(enumeration const &, enumeration const &) -> adl_info<enumeration>;
```

## Usage Example

Define `adl_enum_bounds` for a custom enumeration to specify its bounds:

```cpp
#include <simple_enum/core.hpp>

enum class my_enum { v1, v2, v3 };

// User-defined bounds for my_enum using (ADL)
consteval auto adl_enum_bounds(my_enum) -> simple_enum::adl_info<my_enum> {
    return {my_enum::v1, my_enum::v3}; // Assumes my_enum satisfies enum_concept
}
```

## Notes

- When specializing `adl_enum_bounds`, ensure that the specified first bound is less than or equal to the last bound.
- The function should be defined in the same namespace as the enumeration to utilize ADL effectively.

# `info` Struct Specialization 

## Overview

The `info` struct template enables the specification of custom bounds for enumeration types, allowing users to define the range of valid values for specific enumerations.

## Specialization Requirements

To specialize `simple_enum::info` for an enumeration, define two static constexpr members within the specialization:

- `first`: The minimum value of the enumeration.
- `last`: The maximum value of the enumeration.

These members should be of the enumeration type, and it is crucial that `first <= last` to represent a valid range accurately.

## Example Specialization

The following example demonstrates how to provide custom bounds for the `std::memory_order` enumeration:

```cpp
#include <simple_enum/core.hpp>

template<>
struct simple_enum::info<std::memory_order> {
  static constexpr auto first = std::memory_order::relaxed; // Minimum value
  static constexpr auto last = std::memory_order::seq_cst;  // Maximum value, ensuring first <= last
};

```

# Defining Enumeration Bounds In-Class

## Overview

Enumeration bounds can be directly defined within the enum class itself by using `first` and `last` aliases. This method provides a clear and concise way to specify the range of valid values for an enumeration, facilitating operations that depend on understanding the enumeration's range.

## Requirements

- Define `first` and `last` as aliases within the enumeration.
- Ensure that the value assigned to `first` is less than or equal to the value assigned to `last`.
- Both `first` and `last` should reference valid enumeration values.

## Example

Here's an example of how to define bounds within an enum class for `my_enum`:

```cpp
enum class my_enum { v1, v2, v3, first = v1, last = v3 };
```

- `first = v1`: Indicates the minimum (or first) valid value of the enumeration.
- `last = v3`: Specifies the maximum (or last) valid value of the enumeration.
- The condition `first <= last` is inherently satisfied by this definition.

# Full example

[Full example code available here](https://github.com/arturbac/simple_enum/blob/master/examples/bounded_enums.cc)
