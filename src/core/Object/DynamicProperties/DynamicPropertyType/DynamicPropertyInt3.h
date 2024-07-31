#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"

namespace vg::core
{
    class DynamicPropertyInt3 : public DynamicPropertyT<core::int3>
    {
    public:
        VG_CLASS_DECL(DynamicPropertyInt3, DynamicProperty);

        DynamicPropertyInt3(const core::string & _name = "", core::IObject * _parent = nullptr);

        core::int3 * GetPropertyPtr(const IObject * _object, const IProperty * _prop) const final override;
    };
}