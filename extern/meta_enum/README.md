# Meta Enum - Static reflection on enums in C++17

## Brief

Single-header facility for compile time reflection of enums in C++17.

### Features
 * Automatic string conversion of enum entries
 * Tracking of enum size - i.e. member count
 * Look up enum entries by index
 * Convenience functions for converting between all of the above
 * Supports both enum and enum class
 * Supports enums nested in types/namespaces/functions
 * Keeps track of the C++ code used to declare the enums
 * Single header standard C++17 magic.

### Compiled Example
See compiled code at: https://godbolt.org/z/TaPqPa

### Tested On
 * gcc 7.3 - works
 * gcc 8.1 - works
 * gcc 8.2 - works
 * clang 6.0 - works - however broken with current stdlib version on godbolt's compiler explorer
 * MSVC pre 2018 (version on godbolt's compiler explorer) - works for typical case, not for complex enum test case

## Installation

Either make sure the `meta_enum.hpp` file is made available in your include paths or just copy it to your project.

## Usage

```cpp
#include <meta_enum.hpp>
```

### Declaring Enums

Use the macro `meta_enum` to declare your enums.

```cpp
meta_enum(Days, int, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday);
//equivalent to:
//enum Days : int { Monday, ...
```

Or use `meta_enum_class` for enum class.

The `meta_enum` macro supports assigning custom values to entries as per usual.
```cpp
meta_enum(MyEnum, uint8_t, First = 0, Second = 1 << 5, Third = myConstexprFunction<int>());
```

### Meta Enum Objects

By using `meta_enum` to declare `MyEnum`, you get the global constexpr object `MyEnum_meta` which stores a representation of your enum and is of type `MetaEnum`.

#### MetaEnum type - contains data for the whole enum declaration and holds all members

Typedefs:
 * UnderlyingType - contains the selected underlying integer type for the enum

Members:
 * string - contains the full enum declaration as a `string_view`
 * members - contains representations of all enum members represented as an std::array with MetaEnumMember objects

#### MetaEnumMember type - contains data for one particular enum member

Members:
 * value - The enum value, for example `MyEnum::Second`
 * name - String representation of the member. For example `"Second"`
 * string - The whole declaration string. For example `" Second = 1 << 5"`
 * index - The numerical index of this member. For example `1`

### Convenience functions

A few functions are provided per `meta_enum` to ease usage.

For an enum with the name `MyEnum` you will get the following:
 * `std::string_view MyEnum_value_to_string(MyEnum)` converts an enum value to a textual representation. Will use the `.name` member of the member, or `"__INVALID_ENUM_VAL__"` on invalid input.
 * `std::optional<MetaEnumMember> MyEnum_meta_from_name(std::string_view)` Accesses the meta object for a member found by name. Returns nullopt on invalid input.
 * `std::optional<MetaEnumMember> MyEnum_meta_from_value(MyEnum)` Accesses the meta object for a member found by enum value. Returns nullopt on invalid input.
 * `std::optional<MetaEnumMember> MyEnum_meta_from_index(std::string_view)` Accesses the meta object for a member found by enum member index. Returns nullopt on invalid input.

## Examples

See the file in the repo `examples.cpp`

## Problems and limitations

### Build errors

Some configurations of certain compilers seem to break down when building this. Specifically there is a problem with clang 6.0 on godbolt which fails to build due to what looks like problems with `std::string_view` not being constexpr even though it should be. See: https://godbolt.org/z/Ob9Cnv
There has also been problems when building with the MSVC Pre 2018 hosted @godbolt.org as well when it builds with the complex test case inside of example.cpp. It works however for the typical case. See: https://godbolt.org/z/rIhpfR

### Enum parsing problems with nested templates

`meta_enum` uses string parsing to be able to extract the names of each enum entry. This string parsing takes into account that enum declarations can contain pretty complex meta-programming expressions - it's not as simple as counting commas. The algorithm for doing so is somewhat naive and cannot tell apart `>>` as the operator and `>>` as the end of two nested templates (`std::vector<std::vector<int>> for example`). Due to this, nested template angle brackets have to be separated by whitespace, i.e. `>>` has to be `> >`.

## Contribution

I'm gladly welcoming suggestions for improvements, pull requests, bug reports, comments or whatever. :)
