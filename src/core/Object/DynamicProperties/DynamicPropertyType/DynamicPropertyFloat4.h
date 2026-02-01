#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"

namespace vg::core
{
    class DynamicPropertyFloat4 : public DynamicPropertyT<core::float4>
    {
    public:
        VG_CLASS_DECL(DynamicPropertyFloat4, DynamicPropertyT);

        DynamicPropertyFloat4(const core::string & _name = "", core::IObject * _parent = nullptr);

        core::float4 * GetPropertyPtr(const IObject * _object, const IProperty * _prop) const final override;
    };
}