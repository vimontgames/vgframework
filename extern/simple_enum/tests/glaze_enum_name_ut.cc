// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum
#include <simple_enum/glaze_json_enum_name.hpp>
#include <glaze/ext/jsonrpc.hpp>
#include <simple_enum/ranges_views.hpp>
#include "simple_enum_tests.hpp"
#include <tuple>
#include <ranges>
#include <utility>
#include <algorithm>

namespace views = std::views;
namespace ranges = std::ranges;
enum struct test_enum_e
  {
  foo,
  bar,
  baz
  };

consteval auto adl_enum_bounds(test_enum_e)
  {
  using enum test_enum_e;
  return simple_enum::adl_info{foo, baz};
  }

static_assert(simple_enum::concepts::read_json_supported<test_enum_e>);
static_assert(simple_enum::concepts::write_json_supported<test_enum_e>);

struct test_data_t
  {
  test_enum_e enum_field;
  };

inline constexpr glz::opts pretty{.prettify = true};
enum class Color : int8_t
  {
  Green,
  Blue,
  Red = -1
  };

consteval auto adl_enum_bounds(Color)
  {
  using enum Color;
  return simple_enum::adl_info{Red, Blue};
  }
enum class ColorUnbounded : int8_t
  {
  Green,
  Blue,
  Red = -1
  };

struct complex_request_t
  {
  Color color;
  ColorUnbounded color2;
  int value;
  };

struct complex_response_t
  {
  Color color;
  ColorUnbounded color2;
  test_enum_e enum_field;
  int value;
  std::string name;
  };

struct string_view_test
  {
  std::string_view value;
  Color color;
  };

int main()
  {
  "write_file_json test"_test = []
  {
    string_view_test data{""};
    auto reserr{glz::write_file_json<pretty>(data, std::string{"string_view_test"}, std::string{})};
    expect(reserr.ec == glz::error_code::none);
  };
  "write_file_json test"_test = []
  {
    constexpr test_data_t data{.enum_field = test_enum_e::baz};
    auto reserr{glz::write_file_json<pretty>(data, std::string{"rboxes_file_name"}, std::string{})};
    expect(reserr.ec == glz::error_code::none);

    auto str = glz::write_json(data);
    expect(bool(str));
    if(str)
      expect(eq("{\"enum_field\":\"baz\"}"sv, str.value()));
  };
  "read_file_json test"_test = []
  {
    test_data_t data{test_enum_e::foo};
    auto err{glz::read_file_json(data, std::string{"rboxes_file_name"}, std::string{})};
    expect(err.ec == glz::error_code::none);
    expect(data.enum_field == test_enum_e::baz);
  };

  "write_json_schema test"_test = []
  {
    auto schemares{glz::write_json_schema<test_data_t>()};
    expect(bool(schemares));
    if(schemares)
      {
      std::string schema{std::move(schemares.value())};
      // expect(false) << schema;
      //
      expect(eq(
        schema,
        R"({"type":["object"],"properties":{"enum_field":{"$ref":"#/$defs/test_enum_e"}},"additionalProperties":false,"$defs":{"test_enum_e":{"type":["string"],"oneOf":[{"title":"foo","const":"foo"},{"title":"bar","const":"bar"},{"title":"baz","const":"baz"}]}}})"sv
      ));
      }
  };
  "json rpc call test"_test = []
  {
    glz::rpc::server<glz::rpc::method<"foo", test_data_t, test_data_t>> server;
    glz::rpc::client<glz::rpc::method<"foo", test_data_t, test_data_t>> client;

    server.on<"foo">([](test_data_t const & /*params*/) { return test_data_t{.enum_field = test_enum_e::baz}; });

    std::string uuid{"42"};
    auto [request_str, inserted] = client.request<"foo">(
      uuid,
      test_data_t{.enum_field = test_enum_e::bar},
      [](glz::expected<test_data_t, glz::rpc::error> /*value*/, glz::rpc::id_t /*id*/) -> void
      {
        // Access to value and/or id
      }
    );
    // expect(false) << request_str;
    expect(eq(R"({"jsonrpc":"2.0","method":"foo","params":{"enum_field":"bar"},"id":"42"})"sv, request_str));
    std::string response = server.call(request_str);
    // expect(false) << response;
    expect(eq(R"({"jsonrpc":"2.0","result":{"enum_field":"baz"},"id":"42"})"sv, response));
  };
  "write_json2 test"_test = []
  {
    complex_request_t data{.color = Color::Blue, .color2 = ColorUnbounded::Blue, .value = 3};
    auto err{glz::write_file_json<pretty>(data, std::string{"testfile2"}, std::string{})};
    expect(err.ec == glz::error_code::none);

    auto str = glz::write_json(data);
    expect(bool(str));
    if(str)
      expect(eq(R"({"color":"Blue","color2":1,"value":3})"sv, str.value()));
  };
  "read_file_json test"_test = []
  {
    complex_request_t data{};
    auto err{glz::read_file_json(data, std::string{"testfile2"}, std::string{})};
    expect(err.ec == glz::error_code::none);
    expect(data.color == Color::Blue);
    expect(data.color2 == ColorUnbounded::Blue);
    expect(data.value == 3);
  };
  "json rpc call2 test"_test = []
  {
    glz::rpc::server<glz::rpc::method<"foo", complex_request_t, complex_response_t>> server;
    glz::rpc::client<glz::rpc::method<"foo", complex_request_t, complex_response_t>> client;

    server.on<"foo">(
      [](complex_request_t const & params)
      {
        expect(eq(params.color, Color::Green));
        expect(eq(params.color2, ColorUnbounded::Green));
        return complex_response_t{.color = Color::Red, .color2 = ColorUnbounded::Red, .value = 13};
      }
    );

    std::string uuid{"42"};
    auto [request_str, inserted] = client.request<"foo">(
      uuid,
      complex_request_t{.color = Color::Green, .color2 = ColorUnbounded::Green},
      [](glz::expected<complex_response_t, glz::rpc::error> value, glz::rpc::id_t /*id*/) -> void
      {
        expect(eq(value->color, Color::Red));
        expect(eq(value->color2, ColorUnbounded::Red));
      }
    );
    // expect(false) << request_str;
    expect(
      eq(R"({"jsonrpc":"2.0","method":"foo","params":{"color":"Green","color2":0,"value":0},"id":"42"})"sv, request_str)
    );
    std::string response = server.call(request_str);
    // expect(false) << response;
    expect(eq(
      R"({"jsonrpc":"2.0","result":{"color":"Red","color2":-1,"enum_field":"foo","value":13,"name":""},"id":"42"})"sv,
      response
    ));

    auto reqschemares{glz::write_json_schema<complex_request_t>()};
    expect(bool(reqschemares));
    if(reqschemares)
      std::cout << "request schema :" << reqschemares.value() << std::endl;
  };
  }
