#include "Property.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    Property::Property(const char * _class, const char * _name, Type _type, uint_ptr _offset, core::u32 _value, const char * _prettyName, Flags _flags, uint _enumCount, const char * _enumNames, const void * _enumValues) :
        className(_class),
        name(_name),
        type(_type),
        offset(_offset),
        value(_value),
        displayName(_prettyName),
        flags(_flags)
    {
        if (_enumCount > 0)
        {
            switch (_type)
            {
                case Type::EnumU8:
                case Type::EnumFlagsU8:
                    initEnum<u8>(_enumCount, _enumNames, _enumValues);
                    break;
                case Type::EnumU16:
                case Type::EnumFlagsU16:
                    initEnum<u16>(_enumCount, _enumNames, _enumValues);
                    break;
                case Type::EnumU32:
                case Type::EnumFlagsU32:
                    initEnum<u32>(_enumCount, _enumNames, _enumValues);
                    break;
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(_type);
                    break;
            }
        }
    }

    //--------------------------------------------------------------------------------------
    template <typename T> void Property::initEnum(uint _enumCount, const char * _enumNames, const void * _enumValues)
    {
        enums.resize(_enumCount);
        const char * name = _enumNames;
        for (uint e = 0; e < _enumCount; ++e)
        {
            char temp[256] = { '\0' };
            char * dst = temp;
            while (*name != '\0')
                *dst++ = *name++;
            *dst = '\0';
            ++name;
            enums[e].name = temp;
            enums[e].value = ((T*)_enumValues)[e];
        }
    }

    //--------------------------------------------------------------------------------------
    Property::~Property()
    {
        
    }

    //--------------------------------------------------------------------------------------
    void Property::setRange(float2 _range)
    {
        if ((float)_range.x != (float)_range.y)
            flags |= IProperty::Flags::HasRange;

        range = _range;
    }

    //--------------------------------------------------------------------------------------
    void Property::setFlags(Flags _flagsToSet, Flags _flagsToRemove)
    {
        flags = (Flags)((std::underlying_type<Flags>::type(flags) & ~std::underlying_type<Flags>::type(_flagsToRemove)) | (std::underlying_type<Flags>::type(_flagsToSet)));
    }

    //--------------------------------------------------------------------------------------
    u32 Property::getEnumCount() const
    {
        return enums.count();
    }

    //--------------------------------------------------------------------------------------
    const char * Property::getEnumName(uint index) const
    {
        return enums[index].name.c_str();
    }

    //--------------------------------------------------------------------------------------
    u32 Property::getEnumValue(uint index) const
    {
        return enums[index].value;
    }

    //--------------------------------------------------------------------------------------
    float2 Property::getRange() const
    {
        return range;
    }

    //--------------------------------------------------------------------------------------
    const char * Property::getName() const
    {
        return name;
    }

    //--------------------------------------------------------------------------------------
    const char * Property::getClassName() const
    {
        return className;
    }

    //--------------------------------------------------------------------------------------
    const char * Property::getDisplayName() const
    {
        if (displayName)
            return displayName;
        else
            return name;
    }

    //--------------------------------------------------------------------------------------
    IProperty::Type Property::getType() const
    {
        return type;
    }

    //--------------------------------------------------------------------------------------
    IProperty::Flags Property::getFlags() const
    {
        return flags;
    }

    //--------------------------------------------------------------------------------------
    uint_ptr Property::getOffset() const
    {
        return offset;
    }

    //--------------------------------------------------------------------------------------
    core::u32 Property::getValue() const
    {
        return value;
    }
}