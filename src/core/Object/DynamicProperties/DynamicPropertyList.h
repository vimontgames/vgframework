#pragma once

#include "core/IDynamicPropertyList.h"
#include "DynamicProperty.h"

namespace vg::core
{
    class DynamicPropertyList final : public IDynamicPropertyList
    {
    public:
        VG_CLASS_DECL(DynamicPropertyList, IDynamicPropertyList);

        DynamicPropertyList(const core::string & _name, core::IObject * _parent);
        ~DynamicPropertyList();

        void OnLoad() final override;
        bool RegisterUID() final override;
        bool CanCopyUID() const final override { return true; }
        const IDynamicProperty * GetProperty(const IProperty * _prop) const override;

        core::vector<DynamicProperty*>  m_properties;
    };
}