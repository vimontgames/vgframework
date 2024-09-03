// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum
#ifdef USE_SIMPLE_ENUM
#include <simple_enum/enum_cast.hpp>
using simple_enum::enum_cast;
using simple_enum::enum_name;
#else
#include <magic_enum.hpp>
using magic_enum::enum_cast;
using magic_enum::enum_name;
#endif
#include "enum_definitions.h"

int main()
  {
  (void)enum_cast<lorem_ipsum_long>("exercitation");
  (void)enum_cast<lorem_ipsum_short>("exercitation");
  (void)enum_cast<E>("_11");
  (void)enum_cast<strong_untyped>("v3");
  // for veryfication should reuse same instantations of enum_meta_info_t
  (void)enum_name(lorem_ipsum_long::exercitation);
  (void)enum_name(E::_1);
  (void)enum_name(lorem_ipsum_short::exercitation);
  (void)enum_name(strong_untyped::v1);
  }

