#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"

namespace vg::core
{
    class DynamicPropertyFloat : public DynamicProperty
    {
    public:
        VG_CLASS_DECL(DynamicPropertyFloat, DynamicProperty);

        DynamicPropertyFloat(const core::string & _name = "", core::IObject * _parent = nullptr);
        DynamicPropertyFloat(const core::IObject * _object, const core::IProperty * _prop);
        ~DynamicPropertyFloat();

        bool Set(IObject * _object, IProperty * _prop) final override;

        //private:
        float m_value;
    };
}