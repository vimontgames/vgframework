// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum
#include <simple_enum/simple_enum.hpp>
using simple_enum::enum_name;

#include "enum_definitions.h"

consteval auto adl_enum_bounds(lorem_ipsum_long)
  {
  using enum lorem_ipsum_long;
  return simple_enum::adl_info{eu, asteroides};
  }

// only for benchmarking comparison
int main()
  {
  (void)enum_name(lorem_ipsum_long::exercitation);
  (void)enum_name(E::_1);
  (void)enum_name(lorem_ipsum_short::exercitation);
  (void)enum_name(strong_untyped::v1);
  }

static_assert(simple_enum::limits::min<lorem_ipsum_long>() == lorem_ipsum_long::eu);
static_assert(simple_enum::limits::max<lorem_ipsum_long>() == lorem_ipsum_long::asteroides);
