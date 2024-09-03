// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum
#pragma once

#include <simple_enum/simple_enum.hpp>
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#endif
#include <fmt/format.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include <type_traits>

namespace fmt
  {

template<typename T>
  requires simple_enum::enum_concept<T>
struct formatter<T>
  {
  constexpr auto parse(format_parse_context & ctx) -> decltype(ctx.begin()) { return ctx.begin(); }

  template<typename format_context>
  auto format(T const & e, format_context & ctx) const -> decltype(ctx.out())
    {
    return fmt::format_to(ctx.out(), "{}", simple_enum::enum_name(e));
    }
  };

  }  // namespace fmt

