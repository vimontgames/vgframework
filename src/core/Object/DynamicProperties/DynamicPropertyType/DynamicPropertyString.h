#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"

namespace vg::core
{
    class DynamicPropertyString : public DynamicProperty
    {
    public:
        VG_CLASS_DECL(DynamicPropertyString, DynamicProperty);

        DynamicPropertyString(const core::string & _name = "", core::IObject * _parent = nullptr);
        DynamicPropertyString(const core::IObject * _object, const core::IProperty * _prop);
        ~DynamicPropertyString();

        string m_value;
    };
}