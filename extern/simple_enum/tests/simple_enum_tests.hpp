// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum
#pragma once
#include "simple_enum/simple_enum.hpp"
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wctad-maybe-unsupported"
#pragma clang diagnostic ignored "-Wreserved-identifier"
#pragma clang diagnostic ignored "-Wundef"
#endif

#include <boost/ut.hpp>
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#include "enum_definitions.h"

namespace ut = boost::ut;
using ut::eq;
using ut::expect;
using ut::fatal;
using ut::literals::operator""_test;
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wheader-hygiene"
#endif
using namespace std::string_view_literals;
#ifdef __clang__
#pragma clang diagnostic pop
#endif
std::ostream & operator<<(std::ostream & os, simple_enum::enum_concept auto const & value)
  {
  os << simple_enum::enum_name(value);
  return os;
  }
