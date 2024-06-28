#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"

namespace vg::core
{
    class DynamicPropertyU32 : public DynamicPropertyT<u32>
    {
    public:
        VG_CLASS_DECL(DynamicPropertyU32, DynamicProperty);

        DynamicPropertyU32(const core::string & _name = "", core::IObject * _parent = nullptr);

        u32 * GetPropertyPtr(const IObject * _object, const IProperty * _prop) const final override;
    };
}