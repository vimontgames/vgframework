#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"

namespace vg::core
{
    class DynamicPropertyFloat2 : public DynamicPropertyT<core::float2>
    {
    public:
        VG_CLASS_DECL(DynamicPropertyFloat2, DynamicProperty);

        DynamicPropertyFloat2(const core::string & _name = "", core::IObject * _parent = nullptr);

        core::float2 * GetPropertyPtr(const IObject * _object, const IProperty * _prop) const final override;
    };
}