#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"

namespace vg::core
{
    class DynamicPropertyI32 : public DynamicPropertyT<i32>
    {
    public:
        VG_CLASS_DECL(DynamicPropertyI32, DynamicProperty);

        DynamicPropertyI32(const core::string & _name = "", core::IObject * _parent = nullptr);

        i32 * GetPropertyPtr(const IObject * _object, const IProperty * _prop) const final override;
    };
}