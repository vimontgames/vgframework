#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"

namespace vg::core
{
    class DynamicPropertyInt2 : public DynamicPropertyT<core::int2>
    {
    public:
        VG_CLASS_DECL(DynamicPropertyInt2, DynamicProperty);

        DynamicPropertyInt2(const core::string & _name = "", core::IObject * _parent = nullptr);

        core::int2 * GetPropertyPtr(const IObject * _object, const IProperty * _prop) const final override;
    };
}