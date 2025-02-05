#pragma once

class MetaEnumStringView
{
#define maxNameLength 64U

public:
    constexpr MetaEnumStringView() :
        m_data{},
        m_length(0)
    {
        m_czData[0] = '\0';
    }

    constexpr MetaEnumStringView(const char * _buffer, size_t _length) :
        m_data(_buffer),
        m_length(_length)
    {
        unsigned int index = 0;
        while (',' != *_buffer && '\0' != *_buffer && ' ' != *_buffer && '=' != *_buffer && (index + 1) < maxNameLength)
            m_czData[index++] = *_buffer++;

        m_czData[index] = '\0';
    }

    constexpr const char * data() const { return m_data; }
    constexpr size_t size() const { return m_length; }
    constexpr const char * zData() const { return (const char *)m_czData; }
    constexpr char operator[](std::size_t _pos) const { return m_data[_pos]; }

private:
    const char * m_data;
    size_t m_length = 0;
    char m_czData[maxNameLength] = {};
};
using StringView = MetaEnumStringView;

template <typename T, size_t N> class MetaEnumArray
{
public:
    constexpr const T & operator[](std::size_t _pos) const { return m_data[_pos]; }
    constexpr T & operator[](std::size_t _pos) { return m_data[_pos]; }
    constexpr size_t size() const { return N; }

private:
    T m_data[N];
};

template <typename T, size_t N> using Array = MetaEnumArray<T, N>;

template <typename EnumType>
struct MetaEnumMember
{
    EnumType value = {};
    StringView name;
    size_t index = {};
};

template <typename EnumType, typename UnderlyingTypeIn, size_t size>
struct MetaEnum
{
    using UnderlyingType = UnderlyingTypeIn;
    Array<MetaEnumMember<EnumType>, size> members = {};
};

template <typename E> struct MetaEnumTraits
{
    static_assert(false, "MetaEnumTraits must be specialized for this type using meta_enum_namespace.");
};

namespace meta_enum_internal
{
    constexpr bool isNested(size_t brackets, bool quote)
    {
        return brackets != 0 || quote;
    }

    constexpr size_t nextEnumCommaOrEnd(size_t start, StringView enumString)
    {
        size_t brackets = 0; //()[]{}
        bool quote = false; //""
        char lastChar = '\0';
        char nextChar = '\0';

        auto feedCounters = [&brackets, &quote, &lastChar, &nextChar](char c)
            {
                if (quote)
                {
                    if (lastChar != '\\' && c == '"') //ignore " if they are backslashed
                        quote = false;
                    return;
                }

                switch (c)
                {
                    case '"':
                        if (lastChar != '\\') //ignore " if they are backslashed
                            quote = true;
                        break;
                    case '(':
                    case '<':
                        if (lastChar == '<' || nextChar == '<')
                            break;
                        [[fallthrough]];
                    case '{':
                        ++brackets;
                        break;
                    case ')':
                    case '>':
                        if (lastChar == '>' || nextChar == '>')
                            break;
                        [[fallthrough]];
                    case '}':
                        --brackets;
                        break;
                    default:
                        break;
                }
            };

        size_t current = start;
        for (; current < enumString.size() && (isNested(brackets, quote) || (enumString[current] != ',')); ++current)
        {
            feedCounters(enumString[current]);
            lastChar = enumString[current];
            nextChar = current + 2 < enumString.size() ? enumString[current + 2] : '\0';
        }

        return current;
    }

    constexpr bool isAllowedIdentifierChar(char c)
    {
        return (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            c == '_';
    }

    constexpr StringView parseEnumMemberName(StringView memberString)
    {
        size_t nameStart = 0;
        while (!isAllowedIdentifierChar(memberString[nameStart]))
        {
            ++nameStart;
        }

        size_t nameSize = 0;

        while ((nameStart + nameSize) < memberString.size() && isAllowedIdentifierChar(memberString[nameStart + nameSize]))
        {
            ++nameSize;
        }

        return StringView(memberString.data() + nameStart, nameSize);
    }

    int constexpr length(const char * str)
    {
        return *str ? 1 + length(str + 1) : 0;
    }

    template <typename EnumType, typename UnderlyingType, size_t size>
    constexpr MetaEnum<EnumType, UnderlyingType, size> parseMetaEnum(const char * _in, int _length, const Array<EnumType, size> & values)
    {
        StringView in(_in, _length);
        MetaEnum<EnumType, UnderlyingType, size> result;

        Array<StringView, size> memberStrings;
        size_t amountFilled = 0;

        size_t currentStringStart = 0;

        while (amountFilled < size)
        {
            size_t currentStringEnd = nextEnumCommaOrEnd(currentStringStart + 1, in);
            size_t currentStringSize = currentStringEnd - currentStringStart;

            if (currentStringStart != 0)
            {
                ++currentStringStart;
                --currentStringSize;
            }

            memberStrings[amountFilled] = StringView(in.data() + currentStringStart, currentStringSize);
            ++amountFilled;
            currentStringStart = currentStringEnd;
        }

        for (size_t i = 0; i < memberStrings.size(); ++i)
        {
            result.members[i].name = parseEnumMemberName(memberStrings[i]);
            result.members[i].value = values[i];
            result.members[i].index = i;
        }

        return result;
    }

    template <typename EnumUnderlyingType>
    struct IntWrapper
    {
        constexpr IntWrapper() : value(0), empty(true)
        {
        }
        constexpr IntWrapper(EnumUnderlyingType in) : value(in), empty(false)
        {
        }
        constexpr IntWrapper operator=(EnumUnderlyingType in)
        {
            value = in;
            empty = false;
            return *this;
        }
        EnumUnderlyingType value;
        bool empty;
    };

    template <typename EnumType, typename EnumUnderlyingType, size_t size>
    constexpr Array<EnumType, size> resolveEnumValuesArray(const std::initializer_list<IntWrapper<EnumUnderlyingType>> & in)
    {
        Array<EnumType, size> result{};

        EnumUnderlyingType nextValue = 0;
        for (size_t i = 0; i < size; ++i)
        {
            auto wrapper = *(in.begin() + i);
            EnumUnderlyingType newValue = wrapper.empty ? nextValue : wrapper.value;
            nextValue = newValue + 1;
            result[i] = static_cast<EnumType>(newValue);
        }

        return result;
    }
}

#define meta_enum_global(Type, UnderlyingType, ...)\
    enum Type : UnderlyingType { __VA_ARGS__};\
    constexpr static auto Type##_internal_size = []  () constexpr\
    {\
        using IntWrapperType = meta_enum_internal::IntWrapper<UnderlyingType>;\
        IntWrapperType __VA_ARGS__;\
        return std::initializer_list<IntWrapperType>{__VA_ARGS__}.size();\
    };\
    constexpr static auto Type##_meta = meta_enum_internal::parseMetaEnum<Type, UnderlyingType, Type##_internal_size()>(#__VA_ARGS__, sizeof(#__VA_ARGS__), []() {\
        using IntWrapperType = meta_enum_internal::IntWrapper<UnderlyingType>;\
        IntWrapperType __VA_ARGS__;\
        return meta_enum_internal::resolveEnumValuesArray<Type, UnderlyingType, Type##_internal_size()>({__VA_ARGS__});\
    }());\
    template <> struct MetaEnumTraits<Type>\
    {\
        static const inline auto Meta = Type##_meta; \
    };

#define meta_enum_namespace(Namespace, Type, UnderlyingType, ...)\
        enum Type : UnderlyingType { __VA_ARGS__};\
        constexpr static auto Type##_internal_size = []  () constexpr\
        {\
            using IntWrapperType = meta_enum_internal::IntWrapper<UnderlyingType>;\
            IntWrapperType __VA_ARGS__;\
            return std::initializer_list<IntWrapperType>{__VA_ARGS__}.size();\
        };\
        constexpr static auto Type##_meta = meta_enum_internal::parseMetaEnum<Type, UnderlyingType, Type##_internal_size()>(#__VA_ARGS__, sizeof(#__VA_ARGS__), []() {\
            using IntWrapperType = meta_enum_internal::IntWrapper<UnderlyingType>;\
            IntWrapperType __VA_ARGS__;\
            return meta_enum_internal::resolveEnumValuesArray<Type, UnderlyingType, Type##_internal_size()>({__VA_ARGS__});\
        }());\
    }\
    template <> struct MetaEnumTraits<Namespace::Type>\
    {\
        static const inline auto Meta = Namespace::Type##_meta;\
    };\
    namespace Namespace {

#define meta_enum_class_global(Type, UnderlyingType, ...)\
    enum class Type : UnderlyingType { __VA_ARGS__};\
    constexpr static auto Type##_internal_size = [] () constexpr\
    {\
        using IntWrapperType = meta_enum_internal::IntWrapper<UnderlyingType>;\
        IntWrapperType __VA_ARGS__;\
        return std::initializer_list<IntWrapperType>{__VA_ARGS__}.size();\
    };\
    constexpr static auto Type##_meta = meta_enum_internal::parseMetaEnum<Type, UnderlyingType, Type##_internal_size()>(#__VA_ARGS__, sizeof(#__VA_ARGS__), []() {\
        using IntWrapperType = meta_enum_internal::IntWrapper<UnderlyingType>;\
        IntWrapperType __VA_ARGS__;\
        return meta_enum_internal::resolveEnumValuesArray<Type, UnderlyingType, Type##_internal_size()>({__VA_ARGS__});\
    }());\
    template <> struct MetaEnumTraits<Type>\
    {\
        static const inline auto Meta = Type##_meta;\
    };

#define meta_enum_class_namespace(Namespace, Type, UnderlyingType, ...)\
        enum class Type : UnderlyingType { __VA_ARGS__};\
        constexpr static auto Type##_internal_size = [] () constexpr\
        {\
            using IntWrapperType = meta_enum_internal::IntWrapper<UnderlyingType>;\
            IntWrapperType __VA_ARGS__;\
            return std::initializer_list<IntWrapperType>{__VA_ARGS__}.size();\
        };\
        constexpr static auto Type##_meta = meta_enum_internal::parseMetaEnum<Type, UnderlyingType, Type##_internal_size()>(#__VA_ARGS__, sizeof(#__VA_ARGS__), []() {\
            using IntWrapperType = meta_enum_internal::IntWrapper<UnderlyingType>;\
            IntWrapperType __VA_ARGS__;\
            return meta_enum_internal::resolveEnumValuesArray<Type, UnderlyingType, Type##_internal_size()>({__VA_ARGS__});\
        }());\
    }\
    template <> struct MetaEnumTraits<Namespace::Type>\
    {\
        static const inline auto Meta = Namespace::Type##_meta;\
    };\
    namespace Namespace {

template <typename Type> constexpr const auto & getEnumMembers()
{
    return MetaEnumTraits<Type>::Meta.members;
}

template <typename Type> constexpr const size_t getEnumSize()
{
    return MetaEnumTraits<Type>::Meta.members.size();
}

template <typename Type> const std::string getEnumString(Type e)
{
    const auto & members = getEnumMembers<Type>();
    for (auto i = 0; i < members.size(); ++i)
    {
        const auto & member = members[i];
        if (member.value == e)
            return std::string(member.name.data(), member.name.size());
    }
    return std::string{};
}

template <typename Type> constexpr const char * getEnumCString(Type e)
{
    const auto & members = getEnumMembers<Type>();
    for (auto i = 0; i < members.size(); ++i)
    {
        const auto & member = members[i];
        if (member.value == e)
            return member.name.zData();
    }
    return nullptr;
}

template <typename Type> constexpr Type getEnumValue(unsigned int index)
{
    const auto & members = getEnumMembers<Type>();
    if (index < members.size())
        return members[index].value;
    else
        return (Type)0;
}

