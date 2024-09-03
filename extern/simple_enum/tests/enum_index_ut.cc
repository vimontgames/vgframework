// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum
#include <simple_enum/enum_index.hpp>
#include "simple_enum_tests.hpp"

using simple_enum::enum_index;
using simple_enum::enum_index_error;

int main()
  {
  "enum_index valid"_test = []
  {
    using simple_enum::detail::to_underlying;

    expect(eq(enum_index(lorem_ipsum_short::eu).value(), to_underlying(lorem_ipsum_short::eu)));
    expect(eq(enum_index(lorem_ipsum_short::adipiscing).value(), to_underlying(lorem_ipsum_short::adipiscing)));
    expect(eq(enum_index(lorem_ipsum_short::exercitation).value(), to_underlying(lorem_ipsum_short::exercitation)));

    expect(eq(enum_index(weak::weak_typed_e::v2).value(), 1u));
    expect(eq(enum_index(test::subnamespace::example_3_e::v2).value(), 1u));
      {
      using mf = simple_enum::detail::enum_meta_info_t<weak::weak_typed_e>;
      static_assert(static_cast<weak::weak_typed_e>(mf::first_index()) == weak::v1);
      static_assert(static_cast<weak::weak_typed_e>(mf::last_index()) == weak::v3);
      }
  };

  "enum_index out_of_range"_test = []
  {
    using simple_enum::detail::to_underlying;
    auto out_of_range_value = static_cast<lorem_ipsum_short>(to_underlying(lorem_ipsum_short::last) + 1);
    expect(enum_index(out_of_range_value).error() == enum_index_error::out_of_range);
  };
  }

