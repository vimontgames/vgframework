// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum

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

enum class Color
  {
  Red,
  Green,
  Blue,
  first = Red,
  last = Blue
  };

consteval auto adl_enum_bounds(Color) { return simple_enum::adl_info{Color::Red, Color::Blue}; }

using simple_enum::enum_enumerations;
using simple_enum::enum_name;
using simple_enum::enum_names;
using simple_enum::enum_view;

int main()
  {
  constexpr auto view_over_lorem_ipsum = enum_view<lorem_ipsum>{} | views::transform(enum_name);
  std::cout << "simple_enum " SIMPLE_ENUM_NAME_VERSION "\n";
  for(auto data: view_over_lorem_ipsum)
    std::cout << " " << data << '\n';

  auto color_values = enum_enumerations<Color>();
  for(auto enum_value: color_values)
    std::cout << int(enum_value) << std::endl;

  // Assuming enum_name is a function that converts Color values to their string representations
  auto color_names{enum_names<Color>()};
  for(auto color_name: color_names)
    std::cout << color_name << std::endl;  // Outputs the string representation of each Color value

#if __cplusplus >= 202002L  // C++23 or later

  // Create a zip view of the enumeration values and their names
  auto zipped_view = std::ranges::zip_view(color_values, color_names);

  for(auto [color, name]: zipped_view)
    std::cout << "Color enum: " << static_cast<int>(color) << ", Name: " << name << std::endl;

#endif
  }

/*
simple_enum 0.5.2
 eu
 occaecat
 dolore
 excepteur
 mollit
 adipiscing
 sunt
 ut
 aliqua
 */
