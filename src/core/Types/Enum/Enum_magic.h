#pragma once 

#define MAGIC_ENUM_RANGE_MIN -1
#define MAGIC_ENUM_RANGE_MAX 256
#include "magic_enum/include/magic_enum/magic_enum.hpp"
#include "magic_enum/include/magic_enum/magic_enum_flags.hpp"

namespace vg
{
    namespace core
    {
        template <typename E> inline constexpr auto enumCount()      { return magic_enum::enum_count<E>(); }
        template <typename E> inline constexpr auto enumPairs()      { return magic_enum::enum_entries<E>(); }
        template <typename E> inline constexpr auto isEnumValue(E e) { return magic_enum::enum_contains(e); }
        template <typename E> inline constexpr auto asString(E e)    { auto name = (std::string)magic_enum::enum_name(e); if (name.empty()) { name = (std::string)magic_enum::enum_flags_name(e); } VG_ASSERT(!name.empty(), "Could not retrieve %s enum name for value '%u'", ((std::string)(magic_enum::enum_type_name<E>())).c_str(), magic_enum::enum_integer(e)); return name; }
    }
}

#define vg_enum(Type, UnderlyingType, ...)       enum Type : UnderlyingType { __VA_ARGS__};
#define vg_enum_class(Type, UnderlyingType, ...) enum class Type : UnderlyingType { __VA_ARGS__};