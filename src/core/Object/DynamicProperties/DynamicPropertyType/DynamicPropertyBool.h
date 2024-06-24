#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"

namespace vg::core
{
    class DynamicPropertyBool : public DynamicPropertyT<bool>
    {
    public:
        VG_CLASS_DECL(DynamicPropertyBool, DynamicProperty);

        DynamicPropertyBool(const core::string & _name = "", core::IObject * _parent = nullptr);

        bool * GetPropertyPtr(const IObject * _object, const IProperty * _prop) const final override;
    };
}