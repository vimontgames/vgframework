#pragma once 

#include "meta_enum_lite/include/meta_enum.hpp"

#define vg_enum(Type, UnderlyingType, ...) meta_enum(Type, UnderlyingType, __VA_ARGS__) 
#define vg_enum_class(Type, UnderlyingType, ...) meta_enum_class(Type, UnderlyingType, __VA_ARGS__)

namespace vg
{
    namespace core
    {
        //--------------------------------------------------------------------------------------
        template <typename E> inline constexpr auto operator &(E lhs, E rhs)
        {
            return static_cast<E>(static_cast<std::underlying_type_t<E>>(lhs) & static_cast<std::underlying_type_t<E>>(rhs));
        }

        //--------------------------------------------------------------------------------------
        template <typename E> inline constexpr auto operator |(E lhs, E rhs)
        { 
            return static_cast<E>(static_cast<std::underlying_type_t<E>>(lhs) | static_cast<std::underlying_type_t<E>>(rhs)); 
        }

        //--------------------------------------------------------------------------------------
        template <typename E> inline constexpr auto operator ^(E lhs, E rhs)
        {
            return static_cast<E>(static_cast<std::underlying_type_t<E>>(lhs) ^ static_cast<std::underlying_type_t<E>>(rhs)); 
        }

        //--------------------------------------------------------------------------------------
        template <typename E> inline constexpr auto operator ~(E lhs)
        {
            return static_cast<E>(~static_cast<std::underlying_type_t<E>>(lhs));
        }
                                                                                     
        //--------------------------------------------------------------------------------------
        template <typename E> inline constexpr auto operator &=(E & lhs, E rhs)
        {
            return lhs = lhs & rhs;
        }

        //--------------------------------------------------------------------------------------
        template <typename E> inline constexpr auto operator |=(E & lhs, E rhs)
        {
            return lhs = lhs | rhs; 
        }

        //--------------------------------------------------------------------------------------
        template <typename E> inline constexpr auto operator ^=(E & lhs, E rhs)
        {
            return lhs = lhs ^ rhs; 
        }

        //--------------------------------------------------------------------------------------
        template <typename E> inline constexpr auto asInteger(E e)
        {
            return static_cast<std::underlying_type_t<E>>(e);
        }
        
        //--------------------------------------------------------------------------------------
        template <typename E> inline constexpr bool asBool(E e)
        {
            return 0 != static_cast<std::underlying_type_t<E>>(e);
        }
       
        //--------------------------------------------------------------------------------------
        template <typename E> inline constexpr bool testAnyFlags(E _value, E _flags)
        {
            return 0 != static_cast<std::underlying_type_t<E>>(_value & _flags); 
        }

        //--------------------------------------------------------------------------------------
        template <typename E> inline constexpr bool testAllFlags(E _value, E _flags)
        { 
            return _flags == (_flags & _value); 
        }

        //--------------------------------------------------------------------------------------
        template <typename E> inline constexpr auto enumCount()
        {
            return getEnumMembers<E>().size();
        }

        //--------------------------------------------------------------------------------------
        template <typename E> inline constexpr auto enumPairs()
        {
            const auto & members = getEnumMembers<E>();

            std::vector<std::pair<E, vg::core::string>> pairs;
            pairs.reserve(members.size());

            for (auto i = 0; i < members.size(); ++i)
            {
                const auto & member = members[i];
                pairs.push_back({ member.value, vg::core::string(member.name.data(), member.name.size()) });
            }

            return pairs;
        }

        //--------------------------------------------------------------------------------------
        template <typename E> inline constexpr auto enumValue(uint _index)
        {
            const auto & members = getEnumMembers<E>();
            if (_index < members.size())
                return members[_index].value;
            return (E)0;
        }

        //--------------------------------------------------------------------------------------
        template <typename E> inline constexpr auto isEnumValue(E e)
        {
            const auto & members = getEnumMembers<E>();
            for (auto i = 0; i < members.size(); ++i)
            {
                const auto & member = members[i];
                if (member.value == e)
                    return true;
            }
            return false;
        }

        //--------------------------------------------------------------------------------------
        template <typename E> inline constexpr auto asString(E e)
        {
            return getEnumString(e);
        }

        //--------------------------------------------------------------------------------------
        template <typename E> inline constexpr const char * asCString(E e)
        {
            return getEnumCString(e);
        }
    }
}
  