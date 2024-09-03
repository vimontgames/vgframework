// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum

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

/*
simple_enum 0.5.2 convert string literal to enum and back to string literal [adipiscing]
  excepteur
  aliqua
  occaecat
  eu
  sunt
error during transform: invalid_cast  in_case_of_error
  adipiscing
  ut
  mollit
  dolore
*/
