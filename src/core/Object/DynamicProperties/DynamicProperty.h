#pragma once

#include "core/IDynamicProperty.h"
#include "core/Object/Property.h"

namespace vg::core
{
    class DynamicProperty : public IDynamicProperty
    {
    public:
        VG_CLASS_DECL(DynamicProperty, IDynamicProperty);

        DynamicProperty(const core::string & _name = "", core::IObject * _parent = nullptr);
        ~DynamicProperty();

        IProperty * GetProperty() const final override
        {
            return (IProperty*) &m_prop;
        }

        bool Set(IObject * _object, IProperty * _prop) override
        {
            VG_ASSERT_NOT_IMPLEMENTED();
            return false;
        }

        //core::string m_name;
        Property m_prop = nullptr;
    };
}