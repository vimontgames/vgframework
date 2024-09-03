// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum
#include <magic_enum.hpp>
using magic_enum::enum_name;

#include "enum_definitions.h"

// only for benchmarking comparison

int main()
  {
  (void)enum_name(E::_1);
  (void)enum_name(strong_untyped::v1);
  (void)enum_name(lorem_ipsum_short::exercitation);
  (void)enum_name(lorem_ipsum_long::exercitation);
  }

