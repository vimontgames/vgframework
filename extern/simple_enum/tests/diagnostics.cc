// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum
#include <concepts>
#include <type_traits>
#include <utility>
#include <cstdint>
#include <string_view>
#ifdef __cpp_lib_source_location
#include <source_location>
#endif
#include <array>
#include <iostream>

namespace se
  {
struct meta_name
  {
  char const * data;
  size_t size;
  };

using s = size_t;
using v = void;
  }  // namespace se

void print_compiler_info()
  {
  std::cout << "C++ Standard: " << __cplusplus << std::endl;

#if defined(__clang__)
  std::cout << "Compiler: Clang " << __clang_major__ << '.' << __clang_minor__ << '.' << __clang_patchlevel__
            << std::endl;
#elif defined(__GNUC__)
  std::cout << "Compiler: GCC " << __GNUC__ << '.' << __GNUC_MINOR__ << '.' << __GNUC_PATCHLEVEL__ << std::endl;
#elif defined(__INTEL_COMPILER)
  std::cout << "Compiler: Intel C++ " << __INTEL_COMPILER << std::endl;
#elif defined(_MSC_VER)
  std::cout << "Compiler: MSVC " << _MSC_FULL_VER << std::endl;
#else
  std::cout << "Compiler: Unknown" << std::endl;
#endif

#if defined(_LIBCPP_VERSION)
  std::cout << "Standard Library: libc++ " << _LIBCPP_VERSION << std::endl;
#elif defined(__GLIBCXX__)
  std::cout << "Standard Library: libstdc++ " << __GLIBCXX__ << std::endl;
#elif defined(_MSVC_STL_VERSION)
  std::cout << "Standard Library: MSVC STL " << _MSVC_STL_VERSION << std::endl;
#else
  std::cout << "Standard Library: Unknown" << std::endl;
#endif
  }

namespace se
  {
template<auto enumeration>
constexpr auto f() noexcept
  {
#if defined(__clang__) || defined(__GNUC__)
  char const * const func{__PRETTY_FUNCTION__};
#elif defined(_MSC_VER)
  char const * const func{__FUNCSIG__};
#else
#ifdef __cpp_lib_source_location
  char const * const func{std::source_location::current().function_name()};
#endif
#endif
  return func;
  }

template<auto enumeration>
constexpr auto first_pass(meta_name & res) noexcept
  {
  printf("%s\n", f<enumeration>());
  }
  }  // namespace se
enum struct strong_typed : uint8_t
  {
  v1 = 1,
  v2,
  v3,
  first = v1,
  last = v3
  };
enum struct strong_untyped
  {
  v1 = 1,
  v2,
  v3,
  first = v1,
  last = v3
  };

enum weak_untyped_e
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
  };

namespace subnamespace
  {
  enum struct example_3_e : uint8_t
    {
    };

  namespace detail
    {
    namespace
      {
      enum struct example_4_e : uint8_t
        {
        };
      }
    }  // namespace detail

  inline namespace v2_0
    {
    namespace
      {
      enum struct example_5_e : uint8_t
        {
        };
      }
    }  // namespace v2_0
  }  // namespace subnamespace
  }  // namespace test

// using namespace simple_enum;
int main()
  {
  using std::cout, std::endl;
  print_compiler_info();
  std::cout << "\t" << se::f<strong_typed{}>() << std::endl;
  std::cout << "\t" << se::f<strong_untyped{}>() << std::endl;
  std::cout << "\t" << se::f<weak_untyped_e{}>() << std::endl;
  std::cout << "\t" << se::f<test::strong_typed_2_e{}>() << std::endl;
  std::cout << "\t" << se::f<test::subnamespace::example_3_e{}>() << std::endl;
  std::cout << "\t" << se::f<test::subnamespace::detail::example_4_e{}>() << std::endl;
  std::cout << "\t" << se::f<test::subnamespace::v2_0::example_5_e{}>() << std::endl;
  std::cout << "\t" << se::f<test::subnamespace::example_5_e{}>() << std::endl;
    {
    // constexpr auto v{static_cast<strong_typed>(0)};
    // se::meta_name value{};
    // se::first_pass<static_cast<strong_typed>(0)>(value);
    // se::first_pass<strong_typed::v1>(value);
    //
    // se::first_pass<static_cast<strong_untyped>(0)>(value);
    // se::first_pass<strong_untyped::v1>(value);
    //
    // se::first_pass<static_cast<weak_untyped_e>(0)>(value);
    // se::first_pass<v1>(value);
    }
  }
