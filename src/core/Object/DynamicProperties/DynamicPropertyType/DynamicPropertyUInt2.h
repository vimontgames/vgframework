#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"

namespace vg::core
{
    class DynamicPropertyUInt2 : public DynamicPropertyT<core::uint2>
    {
    public:
        VG_CLASS_DECL(DynamicPropertyUInt2, DynamicPropertyT);

        DynamicPropertyUInt2(const core::string & _name = "", core::IObject * _parent = nullptr);

        core::uint2 * GetPropertyPtr(const IObject * _object, const IProperty * _prop) const final override;
    };
}