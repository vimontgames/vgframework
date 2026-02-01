#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"

namespace vg::core
{
    class DynamicPropertyI64 : public DynamicPropertyT<i64>
    {
    public:
        VG_CLASS_DECL(DynamicPropertyI64, DynamicPropertyT);

        DynamicPropertyI64(const core::string & _name = "", core::IObject * _parent = nullptr);

        i64 * GetPropertyPtr(const IObject * _object, const IProperty * _prop) const final override;
    };
}