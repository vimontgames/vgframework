#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"

namespace vg::core
{
    class DynamicPropertyU16 : public DynamicPropertyT<u16>
    {
    public:
        VG_CLASS_DECL(DynamicPropertyU16, DynamicPropertyT);

        DynamicPropertyU16(const core::string & _name = "", core::IObject * _parent = nullptr);

        u16 * GetPropertyPtr(const IObject * _object, const IProperty * _prop) const final override;
    };
}