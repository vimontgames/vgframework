// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum

#include <simple_enum/core.hpp>         //This header is enough to include for declaring enum bounds
#include <simple_enum/simple_enum.hpp>  // this header is included for enum name
#include <atomic>
#include <iostream>

// simpliest to use bounded enum declaring first and last

enum struct enum_bounded
  {
  v1 = 1500100900,
  v2,
  v3,
  first = v1,
  last = v3
  };

enum struct enum_upper_bounded
  {
  v0,
  v1,
  v2,
  v3,
  last = v3
  };

static void simple_in_enum_bounds()
  {
  // can be evaluated at compile time
  static_assert(simple_enum::enum_name(enum_bounded::v1) == "v1");

  // or at runtime
  auto x0{enum_bounded::v2};
  // enum_bounded has definitions for first and last so compile time is limited to processing meta info for declared
  // range only
  std::cout << "enum_bounded " << simple_enum::enum_name(x0) << "\n";

  // enum_upper_bounded has definitions for last so compile time is limited to processing meta info for range
  // [0..last] range only for upper bounded enum may be sparse enum used with not present first elements including 0
  auto x1{enum_upper_bounded::v2};
  std::cout << "enum_upper_bounded " << simple_enum::enum_name(x1) << "\n";
  }

// lets see example for std::memory_order externally declaring boundary using adl
namespace some_user
  {
enum struct enum_externaly_bounded
  {
  v0 = -15345953,
  v1,
  v2,
  v3
  };

consteval auto adl_enum_bounds(enum_externaly_bounded)
  {
  return simple_enum::adl_info{enum_externaly_bounded::v1, enum_externaly_bounded::v3};
  }

static void enum_externaly_bounded_using_adl()
  {
  auto x1{enum_externaly_bounded::v1};
  std::cout << "enum_externaly_bounded " << simple_enum::enum_name(x1) << "\n";
  }
  }  // namespace some_user

// lets see example for std::memory_order externally declaring boundary using struct specialization
template<>
struct simple_enum::info<std::memory_order>
  {
  /*
  enum class memory_order : int
    {
    relaxed,
    consume,
    acquire,
    release,
    acq_rel,
    seq_cst
    };
  */
  static constexpr auto first = std::memory_order::relaxed;
  static constexpr auto last = std::memory_order::seq_cst;
  };

static void memory_order_externaly_bounding()
  {
  auto x1{std::memory_order::release};
  std::cout << "std::memory_order " << simple_enum::enum_name(x1) << "\n";
  }

int main()
  {
  simple_in_enum_bounds();
  some_user::enum_externaly_bounded_using_adl();
  memory_order_externaly_bounding();
  return EXIT_SUCCESS;
  }

/*
enum_bounded v2
enum_upper_bounded v2
enum_externaly_bounded v1
std::memory_order release
*/
