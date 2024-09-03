// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum
#include <simple_enum/fmtlib_format.hpp>
#include "simple_enum_tests.hpp"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#endif
#include <fmt/format.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

int main()
  {
  "fmtlib formatter test"_test = []
  {
    expect(fmt::format("{}", lorem_ipsum_short::eu) == std::string_view{"eu"});
    expect(fmt::format("{}", lorem_ipsum_short::occaecat) == std::string_view{"occaecat"});
  };
  }
