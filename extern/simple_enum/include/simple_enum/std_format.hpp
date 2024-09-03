// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum
#pragma once

#include <simple_enum/simple_enum.hpp>
#include <type_traits>

#if(defined(__cpp_lib_format) && !(defined(__clang__) && __GLIBCXX__ < 202103L)) \
  || ((_LIBCPP_VERSION >= 1800 || __GLIBCXX__ >= 202103L) && __cplusplus >= 202002L)
#include <format>
#define SIMPLE_ENUM_STD_FORMAT_ENABLED

template<simple_enum::enum_concept enumeration>
struct std::formatter<enumeration>
  {
  template<typename ParseContext>
  constexpr auto parse(ParseContext & ctx)
    {
    return ctx.begin();
    }

  template<typename format_context>
  auto format(enumeration const & e, format_context & ctx) const -> decltype(ctx.out())
    {
    return std::format_to(ctx.out(), "{}", simple_enum::enum_name(e));
    }
  };

#endif  // __cpp_lib_format or C++20

