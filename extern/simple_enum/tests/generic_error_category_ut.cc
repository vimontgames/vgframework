// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum
#include <simple_enum/generic_error_category_impl.hpp>
#include <simple_enum/basic_fixed_string.hpp>
#include "simple_enum_tests.hpp"

using namespace boost::ut;
using namespace simple_enum;
using namespace std::string_view_literals;

namespace
  {
suite basic_fixed_string_tests = []
{
  "basic_fixed_string content test"_test = []
  {
    constexpr basic_fixed_string my_string{"Hello"};
    expect(eq("Hello"sv, my_string.view())) << "String content does not match";
  };

  "basic_fixed_string size test"_test = []
  {
    constexpr basic_fixed_string my_string{"Test"};
    expect(4_u == my_string.size()) << "String size is incorrect";
  };
};

static constexpr basic_fixed_string my_error_str{"ErrorCategoryName"};
using my_error_cat_name_string_literal = string_literal<my_error_str>;

suite string_literal_tests = []
{
  "string_literal value test"_test = []
  {
    expect(eq("ErrorCategoryName"sv, my_error_cat_name_string_literal::value.view()))
      << "String literal value does not match";
    expect(17_u == my_error_cat_name_string_literal::value.size()) << "String literal size is incorrect";
  };
};
  }  // namespace
// Assuming the error category template and other necessary code is defined above

// Example enum for demonstration purposes
enum class test_error_modern
  {
  success = 0,
  failed_other_reason,
  unknown
  };

consteval auto adl_enum_bounds(test_error_modern)
  {
  using enum test_error_modern;
  return simple_enum::adl_info{success, unknown, true};
  }
template class simple_enum::generic_error_category<test_error_modern>;

// old approach on declaring error_code
enum class test_error
  {
  success = 0,
  failed_other_reason,
  unknown
  };

consteval auto adl_enum_bounds(test_error)
  {
  using enum test_error;
  return simple_enum::adl_info{success, unknown};
  }

static_assert(!simple_enum::concepts::declared_error_code<test_error>);

template<>
struct std::is_error_code_enum<test_error> : true_type
  {
  };

// Test cases
static_assert(
  std::is_same_v<decltype(to_camel_case(basic_fixed_string{"hello"})), basic_fixed_string<char, 5>>,
  "Return type mismatch"
);
static_assert(
  to_camel_case(basic_fixed_string{"hello "}) == basic_fixed_string{"Hello "}, "Single word to CamelCase failed"
);
static_assert(
  to_camel_case(basic_fixed_string{"hello world"}) == basic_fixed_string{"Hello World"}, "Two words to CamelCase failed"
);
static_assert(to_camel_case(basic_fixed_string{" "}) == basic_fixed_string{" "}, "Single space handling failed");
static_assert(to_camel_case(basic_fixed_string{" a"}) == basic_fixed_string{" A"}, "Space then char failed");
static_assert(to_camel_case(basic_fixed_string{"a "}) == basic_fixed_string{"A "}, "Char then space failed");
static_assert(
  to_camel_case(basic_fixed_string{"hello world"}) != basic_fixed_string{"hello world"}, "Incorrect CamelCase equality"
);
static_assert(!simple_enum::detail::error_category_name_specialized<test_error>);

enum class test_error_class_spec
  {
  success = 0,
  failed_other_reason,
  unknown
  };

consteval auto adl_enum_bounds(test_error_class_spec)
  {
  using enum test_error_class_spec;
  return simple_enum::adl_info{success, unknown};
  }

template<>
struct std::is_error_code_enum<test_error_class_spec> : true_type
  {
  static constexpr std::string_view category_name = "My Custom Error Category Name";
  };

static_assert(simple_enum::detail::error_category_name_specialized<test_error_class_spec>);

namespace test_adl_decl_error_code
  {
enum class test_adl_decl_error_code
  {
  success = 0,
  failed_other_reason,
  unknown
  };

consteval auto adl_enum_bounds(test_adl_decl_error_code)
  {
  using enum test_adl_decl_error_code;
  return simple_enum::adl_info{success, unknown};
  }

consteval auto adl_decl_error_code(test_adl_decl_error_code) -> bool { return true; }

static_assert(simple_enum::concepts::declared_error_code<test_adl_decl_error_code>);
static_assert(std::is_error_code_enum<test_adl_decl_error_code>::value);
static_assert(!simple_enum::detail::error_category_name_specialized<test_adl_decl_error_code>);
  }  // namespace test_adl_decl_error_code

namespace test_non_adl_decl_error_code
  {
enum class test_non_adl_decl_error_code
  {
  success = 0,
  failed_other_reason,
  unknown
  };

consteval auto adl_enum_bounds(test_non_adl_decl_error_code)
  {
  using enum test_non_adl_decl_error_code;
  return simple_enum::adl_info{success, unknown};
  }

static_assert(!simple_enum::detail::error_category_name_specialized<test_non_adl_decl_error_code>);
static_assert(!std::is_error_code_enum<test_non_adl_decl_error_code>::value);

namespace test_adl_enum_bounds_error_code
  {
  // declare as error enum just using adl_enum_bounds
  enum struct strong_typed_as_error : uint8_t
    {
    v1 = 1,
    v2,
    v3
    };

  consteval auto adl_enum_bounds(strong_typed_as_error)
    {
    using enum strong_typed_as_error;
    return simple_enum::adl_info{v1, v3, true};
    }

  static_assert(adl_enum_bounds(strong_typed_as_error{}).error_code_enum);
  static_assert(simple_enum::detail::has_valid_adl_enum_bounds<strong_typed_as_error>);

  static_assert(simple_enum::detail::adl_info_error_declared_enum<strong_typed_as_error>);
  static_assert(simple_enum::concepts::declared_error_code<strong_typed_as_error>);
  static_assert(std::is_error_code_enum<strong_typed_as_error>::value);
  }  // namespace test_adl_enum_bounds_error_code
  }  // namespace test_non_adl_decl_error_code

namespace
  {
suite modern_error_category_tests = []
{
  using namespace boost::ut;

  "generic_error_category_instance"_test = []
  {
    auto const & instance = generic_error_category<test_error_modern>::instance();
    expect("Test Error Modern"sv == instance.name());
  };

  "make_error_code_success"_test = []
  {
    auto ec = make_error_code(test_error_modern::success);
    expect(eq(0, ec.value()));
    expect(eq("Success"sv, ec.message()));
  };

  "make_error_code_failed"_test = []
  {
    auto ec = make_error_code(test_error_modern::failed_other_reason);
    expect(eq(1, ec.value()));
    expect(eq("Failed Other Reason"sv, ec.message()));
  };

  "make_error_code_unknown"_test = []
  {
    auto ec = make_error_code(test_error_modern::unknown);
    expect(eq(2, ec.value()));
    expect(eq("Unknown"sv, ec.message()));
  };

  "make_unexpected_ec_unknown"_test = []
  {
    auto ec = make_unexpected_ec(test_error_modern::unknown);
    expect(eq(make_error_code(test_error_modern::unknown), ec.error()));
    expect(eq("Unknown"sv, ec.error().message()));
  };
};
suite error_category_tests = []
{
  using namespace boost::ut;

  "generic_error_category_instance"_test = []
  {
    auto const & instance = generic_error_category<test_error>::instance();
    expect("Test Error"sv == instance.name());
  };

  "generic_error_category_instance custom name"_test = []
  {
    auto const & instance = generic_error_category<test_error_class_spec>::instance();
    expect("My Custom Error Category Name"sv == instance.name());
  };

  "make_error_code_success"_test = []
  {
    auto ec = make_error_code(test_error::success);
    expect(eq(0, ec.value()));
    expect(eq("Success"sv, ec.message()));
  };

  "make_error_code_failed"_test = []
  {
    auto ec = make_error_code(test_error::failed_other_reason);
    expect(eq(1, ec.value()));
    expect(eq("Failed Other Reason"sv, ec.message()));
  };

  "make_error_code_unknown"_test = []
  {
    auto ec = make_error_code(test_error::unknown);
    expect(eq(2, ec.value()));
    expect(eq("Unknown"sv, ec.message()));
  };

  "make_unexpected_ec_unknown"_test = []
  {
    auto ec = make_unexpected_ec(test_error::unknown);
    expect(eq(make_error_code(test_error::unknown), ec.error()));
    expect(eq("Unknown"sv, ec.error().message()));
  };
};
  }  // namespace

int main() {}
