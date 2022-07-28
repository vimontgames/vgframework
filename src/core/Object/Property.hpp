#include "Property.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    void Property::setRange(float2 _range)
    {
        if ((float)_range.x != (float)_range.y)
            flags |= IProperty::Flags::HasRange;

        range = _range;
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
    const char * Property::getDisplayName() const
    {
        if (displayName)
            return displayName;
        else
            return name;
    }

    //--------------------------------------------------------------------------------------
    const char * Property::getDescription() const
    {
        return description;
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