#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"

namespace vg::core
{
    class DynamicPropertyFloat : public DynamicPropertyT<float>
    {
    public:
        VG_CLASS_DECL(DynamicPropertyFloat, DynamicProperty);

        DynamicPropertyFloat(const core::string & _name = "", core::IObject * _parent = nullptr);
        ~DynamicPropertyFloat();

        float * GetPropertyPtr(const IObject * _object, const IProperty * _prop) const final override;
    };
}