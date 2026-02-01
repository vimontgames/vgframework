#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"

namespace vg::core
{
    class DynamicPropertyString : public DynamicPropertyT<string>
    {
    public:
        VG_CLASS_DECL(DynamicPropertyString, DynamicPropertyT);

        DynamicPropertyString(const core::string & _name = "", core::IObject * _parent = nullptr);

        string * GetPropertyPtr(const IObject * _object, const IProperty * _prop) const final override;
    };
}