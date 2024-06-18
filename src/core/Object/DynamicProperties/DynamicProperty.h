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

        //core::string m_name;
        Property m_prop = nullptr;
    };
}