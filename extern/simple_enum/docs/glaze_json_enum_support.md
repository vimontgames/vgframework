# Glaze JSON with Enum Names Support

This document provides an overview and examples for integrating and using the extended support for handling enumerations by their names in JSON serialization and deserialization processes, leveraging the [`glaze`](https://github.com/stephenberry/glaze) library in combination with `simple_enum`.

## Including Headers

To start with, include the necessary headers to get access to the functionalities provided by both `simple_enum` and `glaze` libraries:

#include <simple_enum/glaze_json_enum_name.hpp>
#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json.hpp>


## Enabling Serialization By Enum Name

Declare an enumeration, such as `Color`, and define its [bounds](enum_bounds.md). This step is crucial for enabling serialization by enum name:

```cpp
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
```

## Struct for Testing

Create a structure, for instance, `test_struct`, to test the serialization and deserialization processes:

```cpp
struct test_struct
  {
  Color color_field;
  };
```

## JSON Enum Example

Here's how to serialize an enum value to its name and parse it back using `glaze`:
```cpp
static void json_enum_example()
  {
  test_struct data{.color_field = Color::Green};
  auto json_data{glz::write_json(data)};

  std::cout << json_data.value() << std::endl;  // prints {"color_field":"Green"}

  auto parse_res{glz::read_json(data, std::string{R"({"color_field":"Blue"})"})};

  std::cout << std::format("enum value:{}", data.color_field) << std::endl;  // prints enum value:Blue
  }
```

## JSON-RPC Example with Schema

The following example illustrates how to use the JSON RPC implementation with a defined schema including enumeration fields:

```cpp
static void json_rpc_w_schema_example()
  {
  static constexpr glz::string_literal method1_name = "method";
  using rpc_method1 = glz::rpc::method<method1_name, request_t, response_t>;

  glz::rpc::server<rpc_method1> server;
  glz::rpc::client<rpc_method1> client;

  server.on<method1_name>([](request_t const & params) { return response_t{.response_color = params.color}; });

  std::string uuid{"42"};
  auto [request_str, inserted] = client.request<method1_name>(
    uuid,
    request_t{.color = Color::Green, .value ={}},
    [](glz::expected<response_t, glz::rpc::error> value, glz::rpc::id_t id) -> void {}
  );
  
  std::cout << "request :"  << request_str << std::endl;  // prints request JSON string
  std::string response = server.call(request_str);
  
  std::cout << "response :" << response << std::endl;  // prints response JSON string
  
  std::cout << "request schema :" << glz::write_json_schema<request_t>().value() << std::endl;
  }
```

schema returned

```json
{
  "type": [
    "object"
  ],
  "properties": {
    "color": {
      "$ref": "#/$defs/Color"
    },
    "value": {
      "$ref": "#/$defs/int32_t"
    }
  },
  "additionalProperties": false,
  "$defs": {
    "Color": {
      "type": [
        "string"
      ],
      "oneOf": [
        {
          "const": "Red"
        },
        {
          "const": "Green"
        },
        {
          "const": "Blue"
        }
      ]
    },
    "int32_t": {
      "type": [
        "integer"
      ]
    }
  }
}
```

## Special Thanks 

for collaborative effort on making this whole functionality possible

- [Stephen Berry](https://github.com/stephenberry) for awesome glaze library
- [My best friend Jón Bjarni](https://github.com/jbbjarnason) for awesome json rpc extension to glaze
