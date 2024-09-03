
# simple_enum::limits Documentation

The `limits` namespace offers templated function objects, `min` and `max`, aimed at fetching the minimum and maximum values of an enumeration, respectively, with compile-time evaluation.

## Templated Function Objects

### min

- **Type**: `simple_enum::limits::min<enumeration>()`
- **Purpose**: Retrieves the minimum (first) value of a bounded enumeration without requiring an instance for invocation.
- **Usage Example**:

```cpp
#include <simple_enum/simple_enum.hpp>

enum class ExampleEnum { FirstValue = 1, SecondValue, LastValue };

constexpr ExampleEnum minValue = simple_enum::limits::min<ExampleEnum>();
```

### max

- **Type**: `simple_enum::limits::max<enumeration>()`
- **Purpose**: Retrieves the maximum (last) value of a bounded enumeration.
- **Usage Example**:

```cpp
#include <simple_enum/simple_enum.hpp>

enum class ExampleEnum { FirstValue = 1, SecondValue, LastValue };

constexpr ExampleEnum maxValue = simple_enum::limits::max<ExampleEnum>();
```

## Notes

The `min` and `max` function objects are designed for enumeration types that are considered "bounded", having well-defined minimum and maximum values that can be determined at compile time. They enhance the usability and safety of enum types in C++, providing clear, compile-time determinable boundaries for such types.
