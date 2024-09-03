# `enum_index` and `consteval_enum_index`

## Overview

The `enum_index` function object provides functionality for compile time and runtime converting enumeration values to their corresponding indices within a predefined enum range. Additionally, `consteval_enum_index` offers a compile-time variant using template arugment of this functionality.

## Usage 

Include the `simple_enum/enum_index.hpp` in your project to use `enum_index`

## `enum_index` Interface

```cpp
template<enum_concept enum_type>
struct enum_index_t
{
    [[nodiscard]]
    constexpr auto operator()(enum_type value) const noexcept
      -> expected<std::size_t, enum_index_error>;
};
inline constexpr enum_index_t enum_index;
```



- **Usage**: Converts an enumeration value to its index. Returns a `expected<std::size_t, enum_index_error>` indicating success or failure (`out_of_range` error).

## `consteval_enum_index` Function

```cpp
template<enum_concept auto value>
consteval auto consteval_enum_index() -> std::size_t;
```

- **Usage**: A compile-time function that obtains the index of an enum value.

## Error Handling

For enum_index if the provided enumeration value is outside the valid range, an `enum_index_error::out_of_range` error is indicated.
For consteval_enum_index it fails to compile when value is out of the valid range

## Example Usage

```cpp
enum class my_enum { value1, value2, value3 };

// Assuming adl_enum_bounds is defined for my_enum
consteval auto adl_enum_bounds(my_enum) -> simple_enum::adl_info<my_enum> {
    return {my_enum::value1, my_enum::value3};
}

// Using enum_index_t at runtime or compile-time
auto index = enum_index(my_enum::value2);
if(index) {
    std::size_t idx = index.value();
    // Use the index
} else {
    // Handle the out_of_range error
}

// Using consteval_enum_index at compile-time
constexpr std::size_t compile_time_index = consteval_enum_index<my_enum::value2>();
```
