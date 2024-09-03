# `expected_ec` and `make_unexpected_ec`

## Overview

 - `expected_ec` is a template alias simplifying usage of `std::expected` with `std::error_code` for error handling.
 - `make_unexpected_ec` is a utility function creating an `unexpected` object initialized with a `std::error_code` generated from an enumeration representing error conditions.

## Usage

Ensure inclusion .
```cpp
#include <simple_enum/generic_error_category.hpp>
```

## `expected_ec` Type Alias

```cpp
template<typename T>
using expected_ec = std::expected<T, std::error_code>;
```

- **Usage**: Represents a type that may contain a value of type `T` or an error of type `std::error_code`.

## `unexpected_ec` Type Alias

```cpp
using unexpected_ec = std::unexpected<std::error_code>;
```

- **Usage**: Simplifies the declaration of an `unexpected` type carrying a `std::error_code`.

## `make_unexpected_ec` Function

```cpp
template<concepts::error_enum ErrorEnum>
[[nodiscard]] auto make_unexpected_ec(ErrorEnum e) -> unexpected_ec;
```

- **Usage**: Creates an `unexpected_ec` object from an enumeration value `e` representing an error. This leverages `make_error_code` to convert enumeration to `std::error_code`.

## Example Usage

```cpp
enum class error_code { none, error1, error2, error3 };

// Using expected_ec and make_unexpected_ec for error handling
auto func() -> expected_ec<int> {
    if(/* error condition */) {
        return make_unexpected_ec(error_code::error1);
    }
    return 42; // Success path returns a value
}
```

