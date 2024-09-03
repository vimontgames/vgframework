# `enum_name`

## Overview

The `enum_name` function object provides a mechanism to convert enumeration values to their string literals efficiently at compile-time or runtime, leveraging pre-defined metadata.

## Interface Declaration

```cpp
struct enum_name_t
  {
  template<enum_concept enum_type>
  constexpr auto operator()(enum_type value) const noexcept
    -> std::string_view
  };

inline constexpr enum_name_t enum_name;
```

- **Parameters**:
  - `enum_type value`: The enumeration value to convert to a string name.
- **Returns**: 
  - A `std::string_view` representing the name of the enumeration value if within the defined range; otherwise, returns an empty string view.
    
## Usage

Include the `simple_enum/simple_enum.hpp` in your project to use `enum_name`

## Usage Example

Given an enumeration `my_enum` with defined bounds, you can convert an enum value to its string name as follows:

```cpp
#include <simple_enum/simple_enum.hpp>
enum class my_enum { value1, value2, value3 };

// Assuming bounds are defined for `my_enum` using one of 3 methods example below using adl `adl_enum_bounds`
consteval auto adl_enum_bounds(my_enum) -> simple_enum::adl_info<my_enum> {
    return {my_enum::value1, my_enum::value3};
}

// Usage
std::cout << enum_name(my_enum::value2); // Outputs the string representation of `value2`
```
