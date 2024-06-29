#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"

namespace vg::core
{
    class DynamicPropertyI16 : public DynamicPropertyT<i16>
    {
    public:
        VG_CLASS_DECL(DynamicPropertyI16, DynamicProperty);

        DynamicPropertyI16(const core::string & _name = "", core::IObject * _parent = nullptr);

        i16 * GetPropertyPtr(const IObject * _object, const IProperty * _prop) const final override;
    };
}