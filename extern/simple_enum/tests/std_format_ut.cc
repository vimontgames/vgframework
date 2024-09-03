// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum
#include <simple_enum/std_format.hpp>
#include "simple_enum_tests.hpp"
#include <concepts>

int main()
  {
#ifdef SIMPLE_ENUM_STD_FORMAT_ENABLED
  "std::format formatter test"_test = []
  {
    expect(std::format("{}", lorem_ipsum_short::eu) == std::string_view{"eu"});
    expect(std::format("{}", lorem_ipsum_short::occaecat) == std::string_view{"occaecat"});
  };
#endif
  }
