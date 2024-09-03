![BSL](https://img.shields.io/badge/license-BSL1.0-blue.svg) [![CI](https://github.com/arturbac/simple_enum/actions/workflows/ci.yml/badge.svg)](https://github.com/arturbac/simple_enum/actions/workflows/ci.yml)
![C++](https://img.shields.io/badge/C%2B%2B-20%20%7C%2023-blue.svg) ![gcc version](https://img.shields.io/badge/gcc-%3E%3D10-green.svg) ![clang version](https://img.shields.io/badge/clang-%3E%3D13-blue.svg)
![Version](https://img.shields.io/badge/Version-_VERSION_-blue.svg)

# simple_enum

## Features

- **Bounded Enum Views**: Provides `enum_view` for iterating over bounded enumerations, leveraging `std::ranges::views`.
- **Enum to String and Back**: Supports conversion from enum to `std::string_view` and vice versa with minimal compile-time overhead.
- **Enumeration Indexing**: Offers `enum_index`, allowing for index retrieval of enum values based on compile-time metadata.
- **Glaze Integration**: Offers integration with [glaze json and glaze json rpc](https://arturbac.github.io/simple_enum/glaze_json_enum_support.html) 
- **Generic std::error_code** Integration:  refer to [documentation](https://arturbac.github.io/simple_enum/generic_error_category.html)
- **std::format support** for enumerations : refer to [documentation](https://arturbac.github.io/simple_enum/std_format.html)
- **fmt::format support** for enumerations : refer to [documentation](https://arturbac.github.io/simple_enum/fmt_format.html)
- **and more**: take a look at [full documentation here](https://arturbac.github.io/simple_enum/)
```cpp

template<typename type>
concept enum_concept = std::is_enum_v<type>;

struct enum_name_t
  {
  template<enum_concept enum_type>
  static constexpr auto operator()(enum_type value) noexcept;
  };
  
inline constexpr enum_name_t enum_name;

// enum_view can exploit enum_name
constexpr auto view = 
      enum_view<weak_typed_e>{} 
      | views::transform(enum_name);

template<enum_concept enum_type>
struct enum_cast_t
  {
  static constexpr auto operator()(std::string_view value) noexcept
    -> expected<enum_type, enum_cast_error>;
    
template<enum_concept enum_type>
inline constexpr enum_cast_t<enum_type> enum_cast{};

struct enum_index_t
  {
  template<enum_concept enum_type>
  static constexpr auto
    operator()(enum_type value) noexcept -> expected<std::size_t, enum_index_error>;
  };

inline constexpr enum_index_t enum_index;
```
## Motivation

The inception of this project was driven by the noticeable absence of an efficient `enum_name` meta-programming capability in existing solutions. This initiative showcases a proof of concept that `enum_name` can achieve efficiency when the enumeration is bounded. This allows for swift instantiation processes and ensures that the computational cost is directly proportional to the number of enumerations within an enum.

## Current Development Status

`simple_enum` is under active development on **Clang 18**, **GCC 14** and tested on **MSVC** with support on request down to **Clang 13** and**GCC 10**.

Minimum standard required by `simple_enum` is c++20, but it is tested and adopted to exploit c++23 features where it is possible.

## Planned features

- emum bitmasks support
- feature request are possible too
  
## Documentation

[API documentation](https://arturbac.github.io/simple_enum/)

## Examples
 
```cpp
  // There are 3 methods to set enum boundaries
  //inside enum declaration using first last
  enum struct enum_bounded  {  v1 = 1,  v2,  v3,  first = v1,  last = v3  };
  
  // can be evaluated at compile time
  static_assert(simple_enum::enum_name(enum_bounded::v2) == "v2");
  // or at runtime
  auto x0{enum_bounded::v2};
  // enum_bounded has definitions for first and last so compile time is limited to processing meta info for declared
  ut::expect(simple_enum::enum_name(x0) == "v2");

  enum struct enum_upper_bounded  { v0,  v1,  v2,  v3,  last = v3  };

  // enum_upper_bounded has definitions for last so compile time is limited to processing meta info for range
  // [0..last] range only for upper bounded enum may be sparse enum used with not present first elements including 0
  auto x1{enum_upper_bounded::v2};
  ut::expect(simple_enum::enum_name(x1) == "v2");
  
  // externally declaring boundary using adl found function
  namespace some_user
    {
  enum struct enum_externaly_bounded { v0 = -15345953, v1, v2, v3 };

  consteval auto adl_enum_bounds(enum_externaly_bounded)
    {
    return simple_enum::adl_info{enum_externaly_bounded::v1, enum_externaly_bounded::v3};
    }

  static void enum_externaly_bounded_using_adl()
    {
    auto x1{enum_externaly_bounded::v1};
    std::cout << "enum_externaly_bounded " << simple_enum::enum_name(x1) << "\n";
    }
    }  // namespace some_user
    
    
  // externally declaring boundary using struct specialization
  enum class std::memory_order : int
    {
    relaxed,
    //[..]
    seq_cst
    };
    
  template<>
  struct simple_enum::info<std::memory_order>
    {
    static constexpr auto first = std::memory_order::relaxed;
    static constexpr auto last = std::memory_order::seq_cst;
    };
    
  auto x1{std::memory_order::release};
  ut::expect(simple_enum::enum_name(x1) == "release");
  
  // Or just use with unbounded enums as long upper bound doesn't exceeds default_unbounded_upper_range
  ut::expect(simple_enum::enum_name(any_unbounded_enum::value) == "value");
  // default_unbounded_upper_range can be declared by users before inclusion of simple_enum.hpp
  namespace simple_enum
  {
  #define SIMPLE_ENUM_CUSTOM_UNBOUNDED_RANGE
  inline constexpr auto default_unbounded_upper_range = 10;
  }

  //  ranges views compatible enum_view for bounded enumerations
  #include <simple_enum/ranges_views.hpp>

  enum weak_typed_e : uint8_t { v1, v2, v3 };
  
  template<>
  struct simple_enum::info<weak_typed_e>
    {
    static constexpr auto first = weak_typed_e::v1;
    static constexpr auto last = weak_typed_e::v3;
    };
    
  constexpr auto view = enum_view{weak_typed_e::v1, weak_typed_e::v1} 
                       | views::transform(enum_name);
  static constexpr array expected{"v1"sv};
  expect(ranges::equal(view, expected));
    
  constexpr auto view = enum_view<weak_typed_e>{}
                      | views::transform(enum_name);
  static constexpr array expected{"v1"sv, "v2"sv, "v3"sv};
  expect(ranges::equal(view, expected));
  
  // enum_cast function object for enabling the conversion from std::string_view back to the corresponding enumeration type
  #include <simple_enum/enum_cast.hpp>
  enum struct lorem_ipsum { eu, occaecat, dolore, excepteur, mollit, adipiscing, sunt, ut, aliqua, in_case_of_error };
  std::array some_data{
     "excepteur", "aliqua", "occaecat", "eu", "sunt", "__SNEAKY BUG__", "adipiscing", "ut", "mollit", "dolore"};
  auto view_over_lorem_ipsum2 = some_data 
                        | std::views::transform(simple_enum::enum_cast<lorem_ipsum>);
  for ( auto data : view_over_lorem_ipsum2 )
    if(data.has_value())
      std::cout << "  " << enum_name(*data) << '\n';
    else
      std::cout << "error could happen" << enum_name(data.error()) << '\n';
```
## Performance 
How v0.2.0 compares with Instantiation time:
simple enum for bounded enums to magic_enum::enum_name v0.9.5 and reflect::enum_name main/02.02.2024
clang 17 -ftime-trace on AMD Ryzen 9 5900X Linux desktop 6.6.11-gentoo-dist

<table border="1">
  <tr>
    <th>enum</th>
    <th>framework</th>
    <th>enumerations</th>
    <th>instantiation time</th>
    <th>*code gen</th>
    <th>total time</th>
  </tr>
  <tr>
    <td rowspan="3">lorem_ipsum_long</td>
    <td>simple_enum</td>
    <td rowspan="3">104</td>
    <td>12.235 ms</td>
    <td></td>
    <td>12.235 ms</td>
  </tr>
  <tr>
    <td>magic_enum</td>
    <td>199.867 ms</td>
    <td></td>
    <td>199.867 ms</td>
  </tr>
  <tr>
    <td>reflect</td>
    <td>19.282 ms</td>
    <td>36.470 ms</td>
    <td>55.752 ms</td>
  </tr>
  <tr>
    <td rowspan="3">E</td>
    <td>simple_enum</td>
    <td rowspan="3">15</td>
    <td>2.607 ms</td>
    <td></td>
    <td>2.607 ms</td>
  </tr>
  <tr>
    <td>magic_enum</td>
    <td>86.089 ms</td>
    <td></td>
    <td>86.089 ms</td>
  </tr>
  <tr>
    <td>reflect</td>
    <td>19.091 ms</td>
    <td>27.475 ms</td>
    <td>46.566 ms</td>
  </tr>
  <tr>
    <td rowspan="3">lorem_ipsum_short</td>
    <td>simple_enum</td>
    <td rowspan="3">40</td>
    <td>5.286 ms</td>
    <td></td>
    <td>5.286 ms</td>
  </tr>
  <tr>
    <td>magic_enum</td>
    <td>178.864 ms</td>
    <td></td>
    <td>178.864 ms</td>
  </tr>
  <tr>
    <td>reflect</td>
    <td>20.253 ms</td>
    <td>41.024 ms</td>
    <td>61.277 ms</td>
  </tr>
  <tr>
    <td rowspan="3">strong_untyped</td>
    <td>simple_enum</td>
    <td rowspan="3">3</td>
    <td>0.830 ms</td>
    <td></td>
    <td>0.830 ms</td>
  </tr>
  <tr>
    <td>magic_enum</td>
    <td>151.590 ms</td>
    <td></td>
    <td>151.590 ms</td>
  </tr>
  <tr>
    <td>reflect</td>
    <td>22.732 ms</td>
    <td>46.484 ms</td>
    <td>69.216 ms</td>
  </tr>
</table>





*reflect is not fully comparable as part of calculations are moved to runtime and have to be paid at each enum_name call, so in table included is code generation time which is not present for simple and magic.

screen for simple_enum is from ver 0.0.1 , very early so timing is now much better as in table.
![obraz](https://github.com/arturbac/simple_enum/assets/14975842/b2675b3a-491f-49e5-a507-5693d729e06b)

![obraz](https://github.com/arturbac/simple_enum/assets/14975842/e83a0508-9c31-48e2-b841-90e2233d5a5c)

![obraz](https://github.com/arturbac/simple_enum/assets/14975842/db7cb51b-6ba9-45aa-b5a4-fae5049c30df)



 
