
# Examples for simple_enum

This page provides practical examples of how to use `simple_enum` for different scenarios involving enumerations. The examples cover bounded enumerations, enum casting, and utilizing enum views. Each example is self-contained and demonstrates a specific feature of the `simple_enum` library.

## Bounded Enumerations

The `bounded_enums.cc` example demonstrates how to define and use bounded enumerations, which specify explicit first and last values to define a range of valid enum values.

```cpp
#include <simple_enum/simple_enum.hpp>
#include <atomic>
#include <iostream>

// simpliest to use bounded enum declaring first and last

enum struct enum_bounded
  {
  v1 = 1500100900,
  v2,
  v3,
  first = v1,
  last = v3
  };

enum struct enum_upper_bounded
  {
  v0,
  v1,
  v2,
  v3,
  last = v3
  };

static void simple_in_enum_bounds()
  {
  // can be evaluated at compile time
  static_assert(simple_enum::enum_name(enum_bounded::v1) == "v1");

  // or at runtime
  auto x0{enum_bounded::v2};
  // enum_bounded has definitions for first and last so compile time is limited to processing meta info for declared
  // range only
  std::cout << "enum_bounded " << simple_enum::enum_name(x0) << "\n";

  // enum_upper_bounded has definitions for last so compile time is limited to processing meta info for range
  // [0..last] range only for upper bounded enum may be sparse enum used with not present first elements including 0
  auto x1{enum_upper_bounded::v2};
  std::cout << "enum_upper_bounded " << simple_enum::enum_name(x1) << "\n";
  }

// lets see example for std::memory_order externally declaring boundary using adl
namespace some_user
  {
enum struct enum_externaly_bounded
  {
  v0 = -15345953,
  v1,
  v2,
  v3
  };

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

// lets see example for std::memory_order externally declaring boundary using struct specialization
template<>
struct simple_enum::info<std::memory_order>
  {
  /*
  enum class memory_order : int
    {
    relaxed,
    consume,
    acquire,
    release,
    acq_rel,
    seq_cst
    };
  */
  static constexpr auto first = std::memory_order::relaxed;
  static constexpr auto last = std::memory_order::seq_cst;
  };

static void memory_order_externaly_bounding()
  {
  auto x1{std::memory_order::release};
  std::cout << "std::memory_order " << simple_enum::enum_name(x1) << "\n";
  }

int main()
  {
  simple_in_enum_bounds();
  some_user::enum_externaly_bounded_using_adl();
  memory_order_externaly_bounding();
  return EXIT_SUCCESS;
  }

```

[View full example](examples/bounded_enums.cc)

## Enum Cast

The `enum_cast.cc` example illustrates how to convert strings to enum values and vice versa using `enum_cast`.

```cpp
#include <simple_enum/enum_cast.hpp>
#include <ranges>
#include <iostream>

enum struct lorem_ipsum
  {
  eu,
  occaecat,
  dolore,
  excepteur,
  mollit,
  adipiscing,
  sunt,
  ut,
  aliqua,
  in_case_of_error
  };

template<>
struct simple_enum::info<lorem_ipsum>
  {
  static constexpr auto first = lorem_ipsum::eu;
  static constexpr auto last = lorem_ipsum::in_case_of_error;
  };

using simple_enum::enum_cast;
using simple_enum::enum_cast_error;
using simple_enum::enum_name;
using simple_enum::expected;

int main()
  {
  std::cout << "simple_enum " SIMPLE_ENUM_NAME_VERSION " convert string literal to enum and back to string literal ["
            << enum_name(enum_cast<lorem_ipsum>(enum_name(lorem_ipsum::adipiscing)).value()) << "]\n";

  // clang-format off
  std::array some_data{"excepteur", "aliqua", "occaecat", "eu", "sunt", "__SNEAKY BUG__", "adipiscing", "ut", "mollit", "dolore"};
  auto view_over_lorem_ipsum = some_data 
                        | std::views::transform(simple_enum::enum_cast<lorem_ipsum>)
                        | std::views::transform([](expected<lorem_ipsum, enum_cast_error> && result)
                                                { return 
                                                  result.transform(
                                                    [](auto && ev){ return ev; }
                                                       ).transform_error(
                                                    [](auto && ev)
                                                      {
                                                        std::cout << "error during transform: " << enum_name(ev);
                                                        return lorem_ipsum::in_case_of_error; });});
  // clang-format on
  for(auto data: view_over_lorem_ipsum)
    if(data.has_value())
      std::cout << "  " << enum_name(*data) << '\n';
    else
      std::cout << "should not happen" << enum_name(data.error()) << '\n';

  auto view_over_lorem_ipsum2 = some_data | std::views::transform(simple_enum::enum_cast<lorem_ipsum>);
  for(auto data: view_over_lorem_ipsum2)
    if(data.has_value())
      std::cout << "  " << enum_name(*data) << '\n';
    else
      std::cout << "error could happen" << enum_name(data.error()) << '\n';

  return EXIT_SUCCESS;
  }
```

[View full example](examples/enum_cast.cc)

## Enum View

The `enum_view.cc` example showcases how to iterate over enumeration values using `enum_view`, transforming them into strings.

```cpp
#include <simple_enum/ranges_views.hpp>
#include <ranges>
#include <iostream>

namespace views = std::views;
namespace ranges = std::ranges;

enum struct lorem_ipsum
  {
  eu,
  occaecat,
  dolore,
  excepteur,
  mollit,
  adipiscing,
  sunt,
  ut,
  aliqua
  };

template<>
struct simple_enum::info<lorem_ipsum>
  {
  static constexpr auto first = lorem_ipsum::eu;
  static constexpr auto last = lorem_ipsum::aliqua;
  };

using simple_enum::enum_name;
using simple_enum::enum_view;

int main()
  {
  constexpr auto view_over_lorem_ipsum = enum_view<lorem_ipsum>{} | views::transform(enum_name);
  std::cout << "simple_enum " SIMPLE_ENUM_NAME_VERSION "\n";
  for(auto data: view_over_lorem_ipsum)
    std::cout << " " << data << '\n';
  }

```

[View full example](examples/enum_view.cc)

For more detailed explanations and additional examples, please refer to the specific files within the [examples/](examples/) directory.
