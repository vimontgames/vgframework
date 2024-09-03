# `enum_view`

## Overview

`enum_view` is a class template that provides a range view over an enumeration, facilitating iteration over its values. It conforms to `std::ranges::view_interface`, ensuring compatibility with the Standard Library's ranges.

## Requirements

- `enumeration`: The enum type over which the view is created must satisfy the `enum_concept`, ensuring it is an enumeration.

## Constructors

- **Default Constructor**: Initializes the view to cover the entire range of the enumeration. Requires the enumeration to satisfy `bounded_enum`, ensuring that `first` and `last` values are defined.
  
- **Range Constructor**: Initializes the view with custom first and last enumeration values provided.

## Example Usage

```cpp
constexpr auto view_over_lorem_ipsum = enum_view<lorem_ipsum>{} | std::views::transform(enum_name);
std::cout << "simple_enum " SIMPLE_ENUM_NAME_VERSION "\n";
for(auto data: view_over_lorem_ipsum)
    std::cout << " " << data << '\n';
```

This example demonstrates creating an `enum_view` over the `lorem_ipsum` enumeration, transforming it with `enum_name` for string representation, and iterating over the values to print them.

## Deduction Guides

- `enum_view(enumeration) -> enum_view<enumeration>;`
- `enum_view(enumeration, enumeration) -> enum_view<enumeration>;`

These guides assist in the automatic deduction of the `enum_view` template arguments, simplifying its usage.

## Notes

- `enum_view` leverages compile-time metadata to enable efficient iteration over enum ranges.
- It integrates seamlessly with C++20 ranges, offering a modern approach to enum iteration.

