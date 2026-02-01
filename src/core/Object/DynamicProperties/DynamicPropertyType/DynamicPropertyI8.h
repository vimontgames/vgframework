#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"

namespace vg::core
{
    class DynamicPropertyI8 : public DynamicPropertyT<i8>
    {
    public:
        VG_CLASS_DECL(DynamicPropertyI8, DynamicPropertyT);

        DynamicPropertyI8(const core::string & _name = "", core::IObject * _parent = nullptr);

        i8 * GetPropertyPtr(const IObject * _object, const IProperty * _prop) const final override;
    };
}