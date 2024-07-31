#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"

namespace vg::core
{
    class DynamicPropertyUInt4 : public DynamicPropertyT<core::uint4>
    {
    public:
        VG_CLASS_DECL(DynamicPropertyUInt4, DynamicProperty);

        DynamicPropertyUInt4(const core::string & _name = "", core::IObject * _parent = nullptr);

        core::uint4 * GetPropertyPtr(const IObject * _object, const IProperty * _prop) const final override;
    };
}