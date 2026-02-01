#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"

namespace vg::core
{
    class DynamicPropertyFloat4x4 : public DynamicPropertyT<core::float4x4>
    {
    public:
        VG_CLASS_DECL(DynamicPropertyFloat4x4, DynamicPropertyT);

        DynamicPropertyFloat4x4(const core::string & _name = "", core::IObject * _parent = nullptr);

        core::float4x4 * GetPropertyPtr(const IObject * _object, const IProperty * _prop) const final override;
    };
}