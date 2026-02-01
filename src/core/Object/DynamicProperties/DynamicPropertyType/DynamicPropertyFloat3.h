#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"

namespace vg::core
{
    class DynamicPropertyFloat3 : public DynamicPropertyT<core::float3>
    {
    public:
        VG_CLASS_DECL(DynamicPropertyFloat3, DynamicPropertyT);

        DynamicPropertyFloat3(const core::string & _name = "", core::IObject * _parent = nullptr);

        core::float3 * GetPropertyPtr(const IObject * _object, const IProperty * _prop) const final override;
    };
}