#pragma once

#include "core/IDynamicPropertyList.h"
#include "DynamicProperty.h"

namespace vg::core
{
    class DynamicPropertyList : public IDynamicPropertyList
    {
    public:
        VG_CLASS_DECL(DynamicPropertyList, IDynamicPropertyList);

        DynamicPropertyList(const core::string & _name, core::IObject * _parent);
        ~DynamicPropertyList();

        const IDynamicProperty * GetProperty(const IProperty * _prop) const override;

        //core::UID                       m_uid = (core::UID)0;
        //core::string                    m_name;
        core::vector<DynamicProperty*>  m_properties;
    };
}