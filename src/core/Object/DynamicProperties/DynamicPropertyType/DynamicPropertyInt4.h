#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"

namespace vg::core
{
    class DynamicPropertyInt4 : public DynamicPropertyT<core::int4>
    {
    public:
        VG_CLASS_DECL(DynamicPropertyInt4, DynamicPropertyT);

        DynamicPropertyInt4(const core::string & _name = "", core::IObject * _parent = nullptr);

        core::int4 * GetPropertyPtr(const IObject * _object, const IProperty * _prop) const final override;
    };
}