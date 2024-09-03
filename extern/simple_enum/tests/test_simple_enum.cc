// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum

#define SIMPLE_ENNUM_ENABLE_PEN_TEST

#include "simple_enum_tests.hpp"

// TODO chck impact of clang-18 attribute
// enum E { Apple, Orange, Pear };
// struct S {
//   [[clang::preferred_type(E)]] unsigned FruitKind : 2;
// };

// When viewing S::FruitKind in a debugger, it will behave as if the member was declared as type E rather than unsigned.
//

enum struct enum_bounded
  {
  v1 = 1,
  v2,
  v3,
  // first = v1,
  // last = v3
  };

consteval auto adl_enum_bounds(enum_bounded) { return simple_enum::adl_info{enum_bounded::v1, enum_bounded::v3}; }

static_assert(simple_enum::detail::enum_meta_info_t<enum_bounded>::first() == enum_bounded::v1);
static_assert(simple_enum::detail::enum_meta_info_t<enum_bounded>::last() == enum_bounded::v3);

static_assert(simple_enum::limits::min<enum_bounded>() == enum_bounded::v1);
static_assert(simple_enum::limits::max<enum_bounded>() == enum_bounded::v3);

enum struct enum_upper_bounded
  {
  v1 = 1,
  v2,
  v3,
  last = v3
  };
static_assert(simple_enum::detail::enum_meta_info_t<enum_upper_bounded>::first() == static_cast<enum_upper_bounded>(0));
static_assert(simple_enum::detail::enum_meta_info_t<enum_upper_bounded>::last() == enum_upper_bounded::last);

enum struct enum_lower_bounded
  {
  v1 = 9,
  v2,
  first = v1
  };

static_assert(simple_enum::detail::enum_meta_info_t<enum_lower_bounded>::first() == enum_lower_bounded::v1);
static_assert(
  simple_enum::detail::enum_meta_info_t<enum_lower_bounded>::last()
  == static_cast<enum_lower_bounded>(simple_enum::default_unbounded_upper_range)
);

enum struct enum_unbounded
  {
  v1,
  v2,
  v3,
  vx,
  v5,
  v6a
  };

enum struct enum_unbounded_sparse
  {
  v1 = 9,
  v2
  };
static_assert(
  simple_enum::detail::enum_meta_info_t<enum_unbounded_sparse>::first() == static_cast<enum_unbounded_sparse>(0)
);
static_assert(
  simple_enum::detail::enum_meta_info_t<enum_unbounded_sparse>::last()
  == static_cast<enum_unbounded_sparse>(simple_enum::default_unbounded_upper_range)
);

enum weak_global_untyped_e
  {
  v1 = 1,
  v2,
  v3,
  };

consteval auto adl_enum_bounds(weak_global_untyped_e)
  {
  return simple_enum::adl_info{weak_global_untyped_e::v1, weak_global_untyped_e::v3};
  }

static_assert(simple_enum::limits::min<weak_global_untyped_e>() == weak_global_untyped_e::v1);
static_assert(simple_enum::limits::max<weak_global_untyped_e>() == weak_global_untyped_e::v3);

// check for external declarations
enum struct global_untyped_externaly_e
  {
  v1 = 1,
  v2,
  v3,
  };

template<>
struct simple_enum::info<global_untyped_externaly_e>
  {
  static constexpr auto first = global_untyped_externaly_e::v1;
  static constexpr auto last = global_untyped_externaly_e::v3;
  };

static_assert(simple_enum::detail::enum_meta_info_t<global_untyped_externaly_e>::first_index() == 1);
static_assert(simple_enum::detail::enum_meta_info_t<global_untyped_externaly_e>::last_index() == 3);

namespace simple_enum
  {

namespace ut = boost::ut;
enum struct strong_typed : int8_t
  {
  v1 = -121,
  v2,
  v3,
  first = v1,
  last = v3
  };

static_assert(simple_enum::limits::min<strong_typed>() == strong_typed::v1);
static_assert(simple_enum::limits::max<strong_typed>() == strong_typed::v3);

enum struct strong_untyped
  {
  v1 = 1500100900,
  v2,
  v3,
  first = v1,
  last = v3
  };

enum weak_typed_e : uint8_t
  {
  v1 = 1,
  v2,
  v3,
  first = v1,
  last = v3
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
    v1 = 11200,
    v2,
    v3,
    };

  consteval auto adl_enum_bounds(strong_untyped_2_e)
    {
    return simple_enum::adl_info{strong_untyped_2_e::v1, strong_untyped_2_e::v3};
    }

  static_assert(simple_enum::limits::min<strong_untyped_2_e>() == strong_untyped_2_e::v1);
  static_assert(simple_enum::limits::max<strong_untyped_2_e>() == strong_untyped_2_e::v3);

  namespace subnamespace
    {
    enum struct example_3_e : uint8_t
      {
      v1 = 192,
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

static_assert(enumeration_name_v<enum_bounded> == "enum_bounded");
static_assert(enumeration_name_v<enum_upper_bounded> == "enum_upper_bounded");
static_assert(enumeration_name_v<enum_lower_bounded> == "enum_lower_bounded");
static_assert(enumeration_name_v<strong_typed> == "strong_typed");
static_assert(enumeration_name_v<test::strong_typed_2_e> == "strong_typed_2_e");
static_assert(enumeration_name_v<test::strong_untyped_2_e> == "strong_untyped_2_e");
static_assert(enumeration_name_v<test::subnamespace::example_3_e> == "example_3_e");
static_assert(enumeration_name_v<test::subnamespace::detail::example_4_e> == "example_4_e");
static_assert(enumeration_name_v<test::subnamespace::example_5_e> == "example_5_e");
static_assert(enumeration_name_v<test::subnamespace::v2_0::example_5_e> == "example_5_e");

enum struct one_elem_untyped
  {
  v1 = 1,
  first = v1,
  last = v1
  };
enum struct sparse_untyped
  {
  v1 = 1,
  v3 = 3,
  first = v1,
  last = v3
  };
enum struct sparse_offseted_untyped
  {
  unknown = -1,
  v1 = 1,
  v3 = 3,
  first = unknown,  // simulate counting below the range
  last = v3
  };

using detail::cont_pass;
using detail::first_pass;
using detail::meta_name;

template<auto enumeration>
constexpr auto se_view() noexcept -> std::string_view
  {
  using enum_type = std::remove_cvref_t<decltype(enumeration)>;
  meta_name value{};
  size_t beg{first_pass<enum_type::first>(value)};
  cont_pass<enumeration>(value, beg);
  return std::string_view{value.data, value.size};
  }
// " (strong_typed)0]"
// " strong_typed::v1]"
// " strong_typed::v1]"
// "detial::strong_typed::v1]"
// "test::detail::(strong_typed)0]"
// "more_test::test::detial::strong_typed::v1]"

enum struct test_enumeration_name
  {
  first_en = 10,
  sec,
  v2,
  last_en
  };

static ut::suite<"msv tests"> msvc_tests = []
{
  "test auto __cdecl se::f<(enum strong_typed)0x0>(void) noexcept"_test = []
  {
    meta_name result{};
    parse_enumeration_name<'>'>("(enum strong_typed)0x0>", result);
    expect(eq(std::string_view{result.data, result.size}, "strong_typed"sv));
  };

  "test auto __cdecl se::f<(enum strong_untyped)0x0>(void) noexcept"_test = []
  {
    meta_name result{};
    parse_enumeration_name<'>'>("(enum strong_untyped)0x0>", result);
    expect(eq(std::string_view{result.data, result.size}, "strong_untyped"sv));
  };

  "test auto __cdecl se::f<(enum weak_untyped_e)0x0>(void) noexcept"_test = []
  {
    meta_name result{};
    parse_enumeration_name<'>'>("(enum weak_untyped_e)0x0>", result);
    expect(eq(std::string_view{result.data, result.size}, "weak_untyped_e"sv));
  };

  "test auto __cdecl se::f<(enum test::strong_typed_2_e)0x0>(void) noexcept"_test = []
  {
    meta_name result{};
    parse_enumeration_name<'>'>("(enum test::strong_typed_2_e)0x0>", result);
    expect(eq(std::string_view{result.data, result.size}, "strong_typed_2_e"sv));
  };

  "test auto __cdecl se::f<(enum test::subnamespace::example_3_e)0x0>(void) noexcept"_test = []
  {
    meta_name result{};
    parse_enumeration_name<'>'>("(enum test::subnamespace::example_3_e)0x0>", result);
    expect(eq(std::string_view{result.data, result.size}, "example_3_e"sv));
  };

  "test auto __cdecl se::f<(enum test::subnamespace::detail::`anonymous-namespace'::example_4_e)0x0>(void) noexcept"_test
    = []
  {
    meta_name result{};
    parse_enumeration_name<'>'>("(enum test::subnamespace::detail::`anonymous-namespace'::example_4_e)0x0>", result);
    expect(eq(std::string_view{result.data, result.size}, "example_4_e"sv));
  };

  "test auto __cdecl se::f<(enum test::subnamespace::v2_0::`anonymous-namespace'::example_5_e)0x0>(void) noexcept"_test
    = []
  {
    meta_name result{};
    parse_enumeration_name<'>'>("(enum test::subnamespace::v2_0::`anonymous-namespace'::example_5_e)0x0>", result);
    expect(eq(std::string_view{result.data, result.size}, "example_5_e"sv));
  };

  "test auto __cdecl se::f<strong_typed::a>(void) noexcept"_test = []
  {
    meta_name result{};
    parse_enumeration_name<'>'>("strong_typed::a>", result);
    expect(eq(std::string_view{result.data, result.size}, "strong_typed"sv));
  };

  "test auto __cdecl se::f<strong_untyped::a>(void) noexcept"_test = []
  {
    meta_name result{};
    parse_enumeration_name<'>'>("strong_untyped::a>", result);
    expect(eq(std::string_view{result.data, result.size}, "strong_untyped"sv));
  };

  "test auto __cdecl se::f<test::strong_typed_2_e::a>(void) noexcept"_test = []
  {
    meta_name result{};
    parse_enumeration_name<'>'>("test::strong_typed_2_e::a>", result);
    expect(eq(std::string_view{result.data, result.size}, "strong_typed_2_e"sv));
  };

  "test auto __cdecl se::f<test::subnamespace::example_3_e::a>(void) noexcept"_test = []
  {
    meta_name result{};
    parse_enumeration_name<'>'>("test::subnamespace::example_3_e::a>", result);
    expect(eq(std::string_view{result.data, result.size}, "example_3_e"sv));
  };

  "test auto __cdecl se::f<test::subnamespace::detail::`anonymous-namespace'::example_4_e::a>(void) noexcept"_test = []
  {
    meta_name result{};
    parse_enumeration_name<'>'>("test::subnamespace::detail::`anonymous-namespace'::example_4_e::a>", result);
    expect(eq(std::string_view{result.data, result.size}, "example_4_e"sv));
  };

  "test auto __cdecl se::f<test::subnamespace::v2_0::`anonymous-namespace'::example_5_e::a>(void) noexcept"_test = []
  {
    meta_name result{};
    parse_enumeration_name<'>'>("test::subnamespace::v2_0::`anonymous-namespace'::example_5_e::a>", result);
    expect(eq(std::string_view{result.data, result.size}, "example_5_e"sv));
  };
};
static ut::suite<"enumeratioN_name"> enumeratioN_name = []
{
  using namespace ut;
  using namespace std::string_view_literals;
  using namespace simple_enum::detail;

  "test strong_typed from (strong_typed)0]"_test = []
  {
    meta_name result{};
    parse_enumeration_name("(strong_typed)0]", result);
    expect(eq(std::string_view{result.data, result.size}, "strong_typed"sv));
  };

  "test strong_typed from strong_typed::v1]"_test = []
  {
    meta_name result{};
    parse_enumeration_name("strong_typed::v1]", result);
    expect(eq(std::string_view{result.data, result.size}, "strong_typed"sv));
  };
  "test strong_typed from strong_typed::v1]"_test = []
  {
    meta_name result{};
    parse_enumeration_name("strong_typed::v1]", result);
    expect(eq(std::string_view{result.data, result.size}, "strong_typed"sv));
  };
  "test strong_typed from detial::strong_typed::v1]"_test = []
  {
    meta_name result{};
    parse_enumeration_name("de::str::v1]", result);
    expect(eq(std::string_view{result.data, result.size}, "str"sv));
  };

  "test strong_typed from test::detail::(strong_typed)0]"_test = []
  {
    meta_name result{};
    parse_enumeration_name("test::detail::(strong_typed)0]", result);
    expect(eq(std::string_view{result.data, result.size}, "strong_typed"sv));
  };

  "test strong_typed from more_test::test::detial::strong_typed::v1]"_test = []
  {
    meta_name result{};
    parse_enumeration_name("more_test::test::detial::strong_typed::v1]", result);
    expect(eq(std::string_view{result.data, result.size}, "strong_typed"sv));
  };
};

enum class ScopedEnum
  {
  SE_Value1,
  SE_Value2
  };
// Testing an enumeration with no values as a corner case
enum struct EmptyEnum
{
};
// anonymous namespace
enum class MultiValuedEnum
{
  Value1,
  Value2,
  Value3
};
static ut::suite<"enumeration_name_tests"> enumeration_name_tests = []
{
  "ScopedEnum_name"_test = []
  {
    char const * const func{se::f<ScopedEnum{}>()};
    meta_name result;
    parse_enumeration_name<se::end_of_enumeration_name>(func + se::initial_offset + 1, result);
    expect(eq(result.as_view(), "ScopedEnum"sv));
    expect(eq(enumeration_name_v<ScopedEnum>, "ScopedEnum"sv));
  };

  "EmptyEnum_name"_test = []
  {
    char const * const func{se::f<EmptyEnum{}>()};
    meta_name result;
    parse_enumeration_name<se::end_of_enumeration_name>(func + se::initial_offset + 1, result);
    expect(eq(result.as_view(), "EmptyEnum"sv)) << func;
    expect(eq(enumeration_name_v<EmptyEnum>, "EmptyEnum"sv));
  };

  "MultiValuedEnum_name"_test = []
  {
    char const * const func{se::f<MultiValuedEnum{}>()};
    meta_name result;
    parse_enumeration_name<se::end_of_enumeration_name>(func + se::initial_offset + 1, result);
    expect(eq(result.as_view(), "MultiValuedEnum"sv)) << func;
    expect(eq(enumeration_name_v<MultiValuedEnum>, "MultiValuedEnum"sv));
  };
};
static ut::suite<"simple_enum"> _ = []
{
  using namespace ut;
  "test enum_unbounded_sparse offseting"_test = []
  {
    constexpr auto v{static_cast<enum_unbounded_sparse>(0)};
    meta_name value{};
    auto offset{first_pass<v>(value)};
    cont_pass<v>(value, offset);
    cont_pass<enum_unbounded_sparse::v1>(value, offset);
    cont_pass<enum_unbounded_sparse::v2>(value, offset);
  };

  "test unbounded"_test = []
  {
    ut::expect(enum_name(enum_unbounded::v1) == "v1");
    ut::expect(enum_name(enum_unbounded::v2) == "v2");
    ut::expect(enum_name(enum_unbounded::v6a) == "v6a");

    ut::expect(enum_name(enum_unbounded_sparse::v1) == "v1");
    ut::expect(enum_name(enum_unbounded_sparse::v2) == "v2");
  };
  "test variations"_test = []
  {
    ut::expect(enum_name(one_elem_untyped::v1) == "v1");
    ut::expect(enum_name(static_cast<one_elem_untyped>(2)) == "");
    // TESTING UB dosabled
    // ut::expect(se_view<static_cast<sparse_untyped>(2)>() == "2");
    // ut::expect(enum_name(static_cast<sparse_untyped>(2)) == "2");
    ut::expect(enum_name(sparse_offseted_untyped::unknown) == "unknown");
    // TESTING UB dosabled
    // ut::expect(enum_name(static_cast<sparse_offseted_untyped>(0)) == "0");
  };

  "test se meta name cut"_test = []
  {
    // ut::expect(se_view<weak_global_untyped_e::v1>() == "v1");
    // it is out of range or on sparse enum just value when used directly with clang gcc
    // TESTING UB dosabled, testing when first pass for UB is not a case of lower unbounded
    // ut::expect(se_view<static_cast<one_elem_untyped>(2)>() == "2");

    ut::expect(se_view<strong_typed::v1>() == "v1");
    ut::expect(se_view<strong_typed::v2>() == "v2");
    ut::expect(se_view<strong_typed::v3>() == "v3");
    ut::expect(se_view<strong_typed::last>() == "v3");

    ut::expect(se_view<strong_untyped::v1>() == "v1");
    ut::expect(se_view<strong_untyped::v2>() == "v2");
    ut::expect(se_view<strong_untyped::v3>() == "v3");

    ut::expect(se_view<weak_typed_e::v1>() == "v1");
    ut::expect(se_view<weak_typed_e::v2>() == "v2");
    ut::expect(se_view<weak_typed_e::v3>() == "v3");

    ut::expect(se_view<v1>() == "v1");
    ut::expect(se_view<v2>() == "v2");
    ut::expect(se_view<v3>() == "v3");

    ut::expect(se_view<test::strong_typed_2_e::v1>() == "v1");
    ut::expect(se_view<test::strong_typed_2_e::v2>() == "v2");
    ut::expect(se_view<test::strong_typed_2_e::v3>() == "v3");

    ut::expect(se_view<test::subnamespace::example_3_e::v1>() == "v1");
    ut::expect(se_view<test::subnamespace::example_3_e::v2>() == "v2");
    ut::expect(se_view<test::subnamespace::example_3_e::v3>() == "v3");

    ut::expect(se_view<test::subnamespace::detail::example_4_e::v1>() == "v1");
    ut::expect(se_view<test::subnamespace::detail::example_4_e::v2>() == "v2");
    ut::expect(se_view<test::subnamespace::detail::example_4_e::v3>() == "v3");

    ut::expect(se_view<test::subnamespace::example_5_e::v1>() == "v1");
    ut::expect(se_view<test::subnamespace::example_5_e::v2>() == "v2");
    ut::expect(se_view<test::subnamespace::example_5_e::v3>() == "v3");

    ut::expect(se_view<test::subnamespace::weak_typed_5_e::v1>() == "v1");
    ut::expect(se_view<test::subnamespace::weak_typed_5_e::v2>() == "v2");
    ut::expect(se_view<test::subnamespace::weak_typed_5_e::v3>() == "v3");
  };
  "test enum name"_test = []
  {
    ut::expect(enum_name(weak_global_untyped_e::v1) == "v1");

    ut::expect(enum_name(strong_typed::v1) == "v1");
    ut::expect(enum_name(strong_typed::v2) == "v2");
    ut::expect(enum_name(strong_typed::v3) == "v3");

    ut::expect(enum_name(strong_untyped::v1) == "v1");
    ut::expect(enum_name(strong_untyped::v2) == "v2");
    ut::expect(enum_name(strong_untyped::v3) == "v3");

    ut::expect(enum_name(weak_typed_e::v1) == "v1");
    ut::expect(enum_name(weak_typed_e::v2) == "v2");
    ut::expect(enum_name(weak_typed_e::v3) == "v3");

    ut::expect(enum_name(v1) == "v1");
    ut::expect(enum_name(v2) == "v2");
    ut::expect(enum_name(v3) == "v3");

    ut::expect(enum_name(test::strong_typed_2_e::v1) == "v1");
    ut::expect(enum_name(test::strong_typed_2_e::v2) == "v2");
    ut::expect(enum_name(test::strong_typed_2_e::v3) == "v3");

    ut::expect(enum_name(test::subnamespace::example_3_e::v1) == "v1");
    ut::expect(enum_name(test::subnamespace::example_3_e::v2) == "v2");
    ut::expect(enum_name(test::subnamespace::example_3_e::v3) == "v3");

    ut::expect(enum_name(test::subnamespace::detail::example_4_e::v1) == "v1");
    ut::expect(enum_name(test::subnamespace::detail::example_4_e::v2) == "v2");
    ut::expect(enum_name(test::subnamespace::detail::example_4_e::v3) == "v3");

    ut::expect(enum_name(test::subnamespace::example_5_e::v1) == "v1");
    ut::expect(enum_name(test::subnamespace::example_5_e::v2) == "v2");
    ut::expect(enum_name(test::subnamespace::example_5_e::v3) == "v3");
    ut::expect(enum_name(test::subnamespace::v1) == "v1");
    ut::expect(enum_name(test::subnamespace::v2) == "v2");
    ut::expect(enum_name(test::subnamespace::v3) == "v3");
  };
};

  }  // namespace simple_enum

int main() { se::verify_offset(); }
