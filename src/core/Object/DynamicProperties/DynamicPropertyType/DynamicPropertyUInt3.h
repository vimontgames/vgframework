#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"

namespace vg::core
{
    class DynamicPropertyUInt3 : public DynamicPropertyT<core::uint3>
    {
    public:
        VG_CLASS_DECL(DynamicPropertyUInt3, DynamicProperty);

        DynamicPropertyUInt3(const core::string & _name = "", core::IObject * _parent = nullptr);

        core::uint3 * GetPropertyPtr(const IObject * _object, const IProperty * _prop) const final override;
    };
}