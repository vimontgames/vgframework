#pragma once 

#define MAGIC_ENUM_RANGE_MIN -1
#define MAGIC_ENUM_RANGE_MAX 256
#include "magic_enum/include/magic_enum/magic_enum.hpp"
#include "magic_enum/include/magic_enum/magic_enum_flags.hpp"

namespace vg
{
    namespace core
    {
        template <typename E> inline constexpr auto operator &(E lhs, E rhs)         { return static_cast<E>(static_cast<std::underlying_type_t<E>>(lhs) & static_cast<std::underlying_type_t<E>>(rhs)); }
        template <typename E> inline constexpr auto operator |(E lhs, E rhs)         { return static_cast<E>(static_cast<std::underlying_type_t<E>>(lhs) | static_cast<std::underlying_type_t<E>>(rhs)); }
        template <typename E> inline constexpr auto operator ^(E lhs, E rhs)         { return static_cast<E>(static_cast<std::underlying_type_t<E>>(lhs) ^ static_cast<std::underlying_type_t<E>>(rhs)); }
        template <typename E> inline constexpr auto operator ~(E lhs)                { return static_cast<E>(~static_cast<std::underlying_type_t<E>>(lhs)); }
                                                                                     
        template <typename E> inline constexpr auto operator &=(E & lhs, E rhs)      { return lhs = lhs & rhs; }
        template <typename E> inline constexpr auto operator |=(E & lhs, E rhs)      { return lhs = lhs | rhs; }
        template <typename E> inline constexpr auto operator ^=(E & lhs, E rhs)      { return lhs = lhs ^ rhs; }

        template <typename E> inline constexpr auto asInteger(E e)                   { return static_cast<std::underlying_type_t<E>>(e); }
        template <typename E> inline constexpr bool asBool(E e)                      { return 0 != asInteger(e); }
        template <typename E> inline constexpr bool testAnyFlags(E _value, E _flags) { return 0 != static_cast<std::underlying_type_t<E>>(_value & _flags); }
        template <typename E> inline constexpr bool testAllFlags(E _value, E _flags) { return _flags == (_flags & _value); }
    }
}
  
#define ENUM_MAGIC  1
#define ENUM_META   2 

#define ENUM_IMPL ENUM_META

#if ENUM_IMPL == ENUM_MAGIC
#include "Enum/Enum_Magic.h"
#elif ENUM_IMPL == ENUM_META
#include "Enum/Enum_Meta.h"
#endif