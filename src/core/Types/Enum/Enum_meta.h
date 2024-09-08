#pragma once 

#include "meta_enum/include/meta_enum.hpp"

#define vg_enum(Type, UnderlyingType, ...) meta_enum(Type, UnderlyingType, __VA_ARGS__) 
#define vg_enum_class(Type, UnderlyingType, ...) meta_enum_class(Type, UnderlyingType, __VA_ARGS__)

namespace vg
{
    namespace core
    {
        //--------------------------------------------------------------------------------------
        template <typename E> inline constexpr auto enumCount()
        { 
            return getEnumMembers<E>().size();
        }

        //--------------------------------------------------------------------------------------
        template <typename E> inline constexpr auto enumPairs()
        { 
            const auto & members = getEnumMembers<E>();

            std::vector<std::pair<E,vg::core::string>> pairs;
            pairs.reserve(members.size());

            for (auto i = 0; i < members.size(); ++i)
            {
                const auto & member = members[i];
                pairs.push_back({ member.value, vg::core::string(member.name.data(), member.name.size()) });
            }

            return pairs;
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
    }
}