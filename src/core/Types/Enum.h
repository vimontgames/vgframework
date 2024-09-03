#pragma once 

#define MAGIC_ENUM_RANGE_MIN -1
#define MAGIC_ENUM_RANGE_MAX 256
#include "magic_enum/include/magic_enum/magic_enum.hpp"
#include "magic_enum/include/magic_enum/magic_enum_flags.hpp"

namespace vg
{
    namespace core
    {
        template <typename E> inline constexpr auto enumCount()                      { return magic_enum::enum_count<E>(); }
        template <typename E> inline constexpr auto enumValues()                     { return magic_enum::enum_values<E>(); }
        template <typename E> inline constexpr auto enumPairs()                      { return magic_enum::enum_entries<E>(); }
        template <typename E> inline constexpr auto isEnumValue(E e)                 { return magic_enum::enum_contains(e); }

        template <typename E> inline constexpr auto operator &(E lhs, E rhs)         { return static_cast<E>(static_cast<magic_enum::underlying_type_t<E>>(lhs) & static_cast<magic_enum::underlying_type_t<E>>(rhs)); }
        template <typename E> inline constexpr auto operator |(E lhs, E rhs)         { return static_cast<E>(static_cast<magic_enum::underlying_type_t<E>>(lhs) | static_cast<magic_enum::underlying_type_t<E>>(rhs)); }
        template <typename E> inline constexpr auto operator ^(E lhs, E rhs)         { return static_cast<E>(static_cast<magic_enum::underlying_type_t<E>>(lhs) ^ static_cast<magic_enum::underlying_type_t<E>>(rhs)); }
        template <typename E> inline constexpr auto operator ~(E lhs)                { return static_cast<E>(~static_cast<magic_enum::underlying_type_t<E>>(lhs)); }

        template <typename E> inline constexpr auto operator &=(E & lhs, E rhs)      { return lhs = lhs & rhs; }
        template <typename E> inline constexpr auto operator |=(E & lhs, E rhs)      { return lhs = lhs | rhs; }
        template <typename E> inline constexpr auto operator ^=(E & lhs, E rhs)      { return lhs = lhs ^ rhs; }

        template <typename E> inline constexpr auto asString(E e)                    { auto name = (std::string)magic_enum::enum_name(e); if (name.empty()) { name = (std::string)magic_enum::enum_flags_name(e); } VG_ASSERT(!name.empty(), "Could not retrieve %s enum name for value '%u'", ((std::string)(magic_enum::enum_type_name<E>())).c_str(), magic_enum::enum_integer(e)); return name; }
        template <typename E> inline constexpr auto asInteger(E e)                   { return magic_enum::enum_integer(e); }
        template <typename E> inline constexpr bool asBool(E e)                      { return 0 != magic_enum::enum_integer(e); }
        template <typename E> inline constexpr bool testAnyFlags(E _value, E _flags) { return 0 != magic_enum::enum_integer(_value & _flags); }
        template <typename E> inline constexpr bool testAllFlags(E _value, E _flags) { return _flags == (_flags & _value); }
    }
}