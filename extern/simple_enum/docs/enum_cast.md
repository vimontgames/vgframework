# `enum_cast`

## Overview

The `enum_cast` function object facilitates compile-time or runtime conversion from `std::string_view` to enum values.

## Error Handling

Upon encountering a string that doesn't match any enumeration value, `enum_cast_t` yields a `unexpected{enum_cast_error::invalid_cast}`, signaling a conversion failure.

## Interface Declaration

```cpp
template<enum_concept enum_type>
struct enum_cast_t
{
    [[nodiscard]]
    constexpr auto operator()(std::string_view value) const noexcept
      -> expected<enum_type, enum_cast_error>;
};

template<enum_concept enum_type>
inline constexpr enum_cast_t<enum_type> enum_cast{};
```

- **Parameters**:
  - `value`: A `std::string_view` representing the name of the enumeration value.
- **Returns**:
  - A `expected<enum_type, enum_cast_error>` containing the corresponding enum value upon success or an error indicating an invalid cast upon failure.

## Usage

Include the `simple_enum/enum_cast.hpp` in your project to use `enum_cast`.

To convert a `std::string_view` to an enum value, invoke `enum_cast` with the desired enum type and string. The operation returns an `expected` object that either holds the enum value or an error:

```cpp
#include <simple_enum/enum_cast.hpp>

enum class my_enum { value1, value2, value3 };

// Define bounds for `my_enum`
consteval auto adl_enum_bounds(my_enum) -> simple_enum::adl_info<my_enum> {
    return {my_enum::value1, my_enum::value3};
}

// Convert string to `my_enum` value
auto result = enum_cast<my_enum>("value2");
if(result) {
    my_enum e = result.value();
    // Enum value successfully retrieved
} else {
    // Handle the invalid cast
}
```

