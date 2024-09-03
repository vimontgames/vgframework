// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/simple_enum

// header contains support for extending glaze json with handling enumerations by names instead of numbers
// also it contains extended glz::meta<> definitions for bounded enumerations to supply glaze
// with full list of enumeration name/value pairs used at json rpc schema
#include <simple_enum/glaze_json_enum_name.hpp>
#include <simple_enum/std_format.hpp>
#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json.hpp>
#include <iostream>

inline constexpr glz::opts pretty{.prettify = true};

enum class Color
  {
  Red,
  Green,
  Blue
  };

consteval auto adl_enum_bounds(Color)
  {
  using enum Color;
  return simple_enum::adl_info{Red, Blue};
  }

struct test_struct
  {
  Color color_field;
  };

static void json_enum_example()
  {
  // when this header is included then serialization by enum name is enabled by default for any enum fulfilling
  // bounded_enum
  test_struct data{.color_field = Color::Green};
  auto json_data{glz::write_json(data)};
  if(json_data)
    std::cout << json_data.value() << std::endl;
  // prints {"color_field":"Green"}

  [[maybe_unused]]
  auto parse_res{glz::read_json(data, std::string{R"({"color_field":"Blue"})"})};

  std::cout << std::format("enum value:{}", data.color_field) << std::endl;
  // prints enum value:Blue
  }

struct request_t
  {
  Color color;
  int value;
  };

struct response_t
  {
  Color response_color;
  };

static void json_rpc_w_schema_example()
  {
  // json rpc example and schema
  static constexpr glz::string_literal method1_name = "method";
  using rpc_method1 = glz::rpc::method<method1_name, request_t, response_t>;

  glz::rpc::server<rpc_method1> server;
  glz::rpc::client<rpc_method1> client;

  server.on<method1_name>([](request_t const & params) { return response_t{.response_color = params.color}; });

  std::string uuid{"42"};
  auto [request_str, inserted] = client.request<method1_name>(
    uuid,
    request_t{.color = Color::Green, .value = {}},
    [](glz::expected<response_t, glz::rpc::error> value, glz::rpc::id_t id) -> void
    {
      // Access to value and/or id
    }
  );

  std::cout << "request :" << request_str << std::endl;
  // prints request :{"jsonrpc":"2.0","method":"method","params":{"color":"Green","value":0},"id":"42"}
  std::string response = server.call(request_str);

  std::cout << "response :" << response << std::endl;
  // prints response :{"jsonrpc":"2.0","result":{"response_color":"Green"},"id":"42"}

  std::cout << "request schema :" << glz::write_json_schema<request_t>().value() << std::endl;
  // {
  //   "type": [
  //     "object"
  //   ],
  //   "properties": {
  //     "color": {
  //       "$ref": "#/$defs/Color"
  //     },
  //     "value": {
  //       "$ref": "#/$defs/int32_t"
  //     }
  //   },
  //   "additionalProperties": false,
  //   "$defs": {
  //     "Color": {
  //       "type": [
  //         "string"
  //       ],
  //       "oneOf": [
  //         {
  //           "const": "Red"
  //         },
  //         {
  //           "const": "Green"
  //         },
  //         {
  //           "const": "Blue"
  //         }
  //       ]
  //     },
  //     "int32_t": {
  //       "type": [
  //         "integer"
  //       ]
  //     }
  //   }
  // }
  std::cout << "response schema :" << glz::write_json_schema<response_t>().value() << std::endl;
  // {
  //   "type": [
  //     "object"
  //   ],
  //   "properties": {
  //     "response_color": {
  //       "$ref": "#/$defs/Color"
  //     }
  //   },
  //   "additionalProperties": false,
  //   "$defs": {
  //     "Color": {
  //       "type": [
  //         "string"
  //       ],
  //       "oneOf": [
  //         {
  //           "const": "Red"
  //         },
  //         {
  //           "const": "Green"
  //         },
  //         {
  //           "const": "Blue"
  //         }
  //       ]
  //     }
  //   }
  // }
  }

int main()
  {
  json_enum_example();
  json_rpc_w_schema_example();
  }
