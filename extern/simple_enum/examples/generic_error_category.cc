// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum

#include <simple_enum/generic_error_category.hpp>
#include <simple_enum/expected.h>
#include <simple_enum/fmtlib_format.hpp>

// >>>>>-----[[[***]]]-----<<<<<>>>>>-----[[[***]]]-----<<<<<>>>>>-----[[[***]]]-----<<<<<>>>>>-----[[[***]]]-----<<<<<>
//
//                                             Header Declaration Section
//
// >>>>>-----[[[***]]]-----<<<<<>>>>>-----[[[***]]]-----<<<<<>>>>>-----[[[***]]]-----<<<<<>>>>>-----[[[***]]]-----<<<<<>

namespace custom_error_example
  {
enum class function_error
  {
  failed_other_reason,
  unhandled_exaption
  };

// There are 3 ways to declare enum as error_code
#define USE_ADL_WAY_TO_DECL_ERROR_CODE 1

#if USE_ADL_WAY_TO_DECL_ERROR_CODE == 1
// as part of adl_enum_bounds passing true as last constructor argument
consteval auto adl_enum_bounds(function_error)
  {
  using enum function_error;
  return simple_enum::adl_info{failed_other_reason, unhandled_exaption, true};
  }

#elif USE_ADL_WAY_TO_DECL_ERROR_CODE == 2
// old way of adl_decl_error_code separated from adl_enum_bounds
consteval auto adl_enum_bounds(function_error)
  {
  using enum function_error;
  return simple_enum::adl_info{failed_other_reason, unhandled_exaption};
  }

// using provided by simple_enum partial specialization of std::is_error_code_enum
consteval auto adl_decl_error_code(function_error) -> bool { return true; }
#endif
  }  // namespace custom_error_example

// or declaring it Yourself with optional ability to customise category name
#if !USE_ADL_WAY_TO_DECL_ERROR_CODE
template<>
struct std::is_error_code_enum<custom_error_example::function_error> : true_type
  {
  // optional if not defined Camel space Cased enumeration type name is used
  static constexpr std::string_view category_name = "My Custom Error Category Name";
  };
#endif
namespace custom_error_example
  {

using function_error_category = simple_enum::generic_error_category<function_error>;

  }  // namespace custom_error_example

extern template class simple_enum::generic_error_category<custom_error_example::function_error>;

// >>>>>-----[[[***]]]-----<<<<<>>>>>-----[[[***]]]-----<<<<<>>>>>-----[[[***]]]-----<<<<<>>>>>-----[[[***]]]-----<<<<<>
//
//                                    Embeddable only once in any translation unit
//
// >>>>>-----[[[***]]]-----<<<<<>>>>>-----[[[***]]]-----<<<<<>>>>>-----[[[***]]]-----<<<<<>>>>>-----[[[***]]]-----<<<<<>
#include <simple_enum/generic_error_category_impl.hpp>

using custom_error_example::function_error;

// The design of std::error_code necessitates an instance of error_category, and given that the base class includes a
// virtual destructor, it cannot be declared inline constexpr.
// With clang however destruction is omitted with attribute no_destroy

template class simple_enum::generic_error_category<function_error>;

// >>>>>-----[[[***]]]-----<<<<<>>>>>-----[[[***]]]-----<<<<<>>>>>-----[[[***]]]-----<<<<<>>>>>-----[[[***]]]-----<<<<<>
//
//                                                 Example use
//
// >>>>>-----[[[***]]]-----<<<<<>>>>>-----[[[***]]]-----<<<<<>>>>>-----[[[***]]]-----<<<<<>>>>>-----[[[***]]]-----<<<<<>
namespace custom_error_example
  {
using cxx23::expected;
using cxx23::unexpected;

static auto my_function(int arg) -> expected<void, std::error_code>
try
  {
  if(arg != 0)
    // simple_enum::make_error_code or function_error_category::make_error_code can be used
    return unexpected{simple_enum::make_error_code(function_error::failed_other_reason)};
  return {};
  }
catch(...)
  {
  return unexpected{function_error_category::make_error_code(function_error::unhandled_exaption)};
  }
  }  // namespace custom_error_example

int main(int argc, char const * const *)
  {
  using namespace custom_error_example;
  auto res{my_function(argc)};
  if(!res)
    {
    std::error_code ec{std::move(res.error())};
    fmt::print(
      "Error Category:\"{}\" message:\"{}\" enumeration value:\"{}\"",
      ec.category().name(),
      ec.message(),
      function_error_category::enumeration(ec.value())
    );
    }

  return EXIT_SUCCESS;
  }
