// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum
#pragma once

#include <cstdint>

// clang-format off
enum struct E : uint8_t
  {
  _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, first = _1, last = _16
  };
enum struct lorem_ipsum_short
{
  eu, occaecat, dolore, excepteur, mollit, adipiscing, sunt, ut, aliqua, quis, ad, proident, fugiat, eiusmod, magna, nisi, duis, minim, nulla, officia, amet, sit, voluptate, velit, deserunt, cillum, in, laboris, aute, lorem, incididunt, culpa, cupidatat, elit, sed, sint, nostrud, do_, et, exercitation, first = eu, last = exercitation
};
enum struct lorem_ipsum_long
{
  eu = 10, occaecat, dolore, excepteur, mollit, adipiscing, sunt, ut, aliqua, quis, ad, proident, fugiat, eiusmod, magna, nisi, duis, minim, nulla, officia, amet, sit, voluptate, velit, deserunt, cillum, in, laboris, aute, lorem, incididunt, culpa, cupidatat, elit, sed, sint, nostrud, do_, et, exercitation, pariatur, ullamco, aliquip, esse, anim, commodo, est, tempor, enim, ipsum, non, labore, consequat, id, consectetur, qui, veniam, ex, reprehenderit, laborum, dolor, ea, irure, veritas, virtus, vis, vita, vivere, vox, vulnus, terra, caelum, aqua, ignis, spiritus, amor, pax, bellum, lux, tenebrae, sol, luna, stella, caelestis, infernus, aether, aer, ventus, flamma, umbra, aurora, crepusculum, sidus, astrum, galaxias, nebula, cosmos, universum, planetarium, solaris, lunaris, stellarum, cometes, asteroides, first = eu, last = asteroides
};

// clang-format on

constexpr lorem_ipsum_short & operator++(lorem_ipsum_short & value) noexcept
  {
  return value = static_cast<lorem_ipsum_short>(static_cast<std::underlying_type_t<lorem_ipsum_short>>(value) + 1);
  }

constexpr lorem_ipsum_short operator++(lorem_ipsum_short & value, int) noexcept
  {
  lorem_ipsum_short originalValue = value;
  ++value;
  return originalValue;
  }

constexpr lorem_ipsum_long & operator++(lorem_ipsum_long & value) noexcept
  {
  return value = static_cast<lorem_ipsum_long>(static_cast<std::underlying_type_t<lorem_ipsum_long>>(value) + 1);
  }

constexpr lorem_ipsum_long operator++(lorem_ipsum_long & value, int) noexcept
  {
  lorem_ipsum_long originalValue = value;
  ++value;
  return originalValue;
  }

auto fn = []
{
  using type = lorem_ipsum_short;
  constexpr size_t size = size_t(type::last) - size_t(type::first) + 1;
  std::array<std::string_view, size> result{};
  for(auto it{type::first}; type::last >= it; ++it)
    result[size_t(it) - size_t(type::first)] = simple_enum::enum_name(it);
  return result;
};

enum struct strong_typed : uint8_t
  {
  v1 = 1,
  v2,
  v3
  };

consteval auto adl_enum_bounds(strong_typed)
  {
  using enum strong_typed;
  return simple_enum::adl_info{v1, v3};
  }

enum struct strong_untyped
  {
  v1 = 1,
  v2,
  v3,
  first = v1,
  last = v3
  };

template<>
struct simple_enum::info<strong_untyped>
  {
  static constexpr auto first = strong_untyped::v1;
  static constexpr auto last = strong_untyped::v3;
  };

namespace weak
  {
enum weak_typed_e : int8_t
  {
  v1 = -3,
  v2,
  v3
  };
  }

template<>
struct simple_enum::info<weak::weak_typed_e>
  {
  static constexpr auto first = weak::weak_typed_e::v1;
  static constexpr auto last = weak::weak_typed_e::v3;
  };

namespace test
  {
enum struct strong_typed_2_e : int16_t
  {
  v1 = 1,
  v2,
  v3,
  first = v1,
  last = v3
  };

enum struct strong_untyped_2_e
  {
  v1 = 1,
  v2,
  v3,
  first = v1,
  last = v3
  };

namespace subnamespace
  {
  enum struct example_3_e : uint8_t
    {
    v1 = 5,
    v2,
    v3,
    first = v1,
    last = v3
    };

  enum weak_untyped_3_e
    {
    v1 = 1,
    v2,
    v3,
    first = v1,
    last = v3
    };

  namespace detail
    {
    namespace
      {
      enum struct example_4_e : uint8_t
        {
        v1 = 1,
        v2,
        v3,
        first = v1,
        last = v3
        };
      }
    }  // namespace detail

  inline namespace v2_0
    {
    namespace
      {
      enum struct example_5_e : uint8_t
        {
        v1 = 1,
        v2,
        v3,
        first = v1,
        last = v3
        };

      enum weak_typed_5_e : uint8_t
        {
        v1 = 1,
        v2,
        v3,
        first = v1,
        last = v3
        };
      }  // namespace
    }  // namespace v2_0
  }  // namespace subnamespace
  }  // namespace test
