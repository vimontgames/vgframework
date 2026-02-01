#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"

namespace vg::core
{
    class DynamicPropertyU64 : public DynamicPropertyT<u64>
    {
    public:
        VG_CLASS_DECL(DynamicPropertyU64, DynamicPropertyT);

        DynamicPropertyU64(const core::string & _name = "", core::IObject * _parent = nullptr);

        u64 * GetPropertyPtr(const IObject * _object, const IProperty * _prop) const final override;
    };
}