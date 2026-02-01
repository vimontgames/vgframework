#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"

namespace vg::core
{
    class DynamicPropertyU8 : public DynamicPropertyT<u8>
    {
    public:
        VG_CLASS_DECL(DynamicPropertyU8, DynamicPropertyT);

        DynamicPropertyU8(const core::string & _name = "", core::IObject * _parent = nullptr);

        u8 * GetPropertyPtr(const IObject * _object, const IProperty * _prop) const final override;
    };
}