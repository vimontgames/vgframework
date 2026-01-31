#include "DynamicPropertyList.h"

namespace vg::core
{
    VG_REGISTER_CLASS(DynamicPropertyList, "DynamicPropertyList");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyList::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyObjectPtrVector(DynamicPropertyList, m_properties, "Properties");
        //setPropertyFlag(DynamicPropertyList, m_uid, PropertyFlags::Hidden | PropertyFlags::Debug, false);
        //setPropertyFlag(DynamicPropertyList, m_originalUID, PropertyFlags::Hidden | PropertyFlags::Debug, false);

        setPropertyFlag(DynamicPropertyList, m_originalUID, PropertyFlags::Transient, false);

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyList::DynamicPropertyList(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {
        
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyList::~DynamicPropertyList()
    {
        for (uint i = 0; i < m_properties.size(); ++i)
            VG_SAFE_DELETE(m_properties[i]);
        m_properties.clear();
    }

    //--------------------------------------------------------------------------------------
    void DynamicPropertyList::OnLoad()
    {
        fixOriginalUID();

        for (uint i = 0; i < m_properties.size(); ++i)
            m_properties[i]->SetParent(this);
    }

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyList::fixOriginalUID()
    {
        if (!GetOriginalUID(false))
        {
            SetOriginalUID(GetUID());
            SetUID(0x0);
            super::RegisterUID();

            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    void DynamicPropertyList::OnSave()
    {
        fixOriginalUID();
    }

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyList::RegisterUID()
    {
        return false;
    }

    //--------------------------------------------------------------------------------------
    const IDynamicProperty * DynamicPropertyList::GetProperty(const IProperty * _prop) const
    {
        for (uint i = 0; i < m_properties.size(); ++i)
        {
            auto & prop = m_properties[i];
            if (prop->GetName() == _prop->GetName())
            {
                prop->CopyProperty(_prop);
                return prop;
            }
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    core::UID DynamicPropertyList::getRefUID() const
    {
        if (GetOriginalUID(false))
        {
            return GetOriginalUID();
        }
        else
        {
            VG_WARNING("[Prefab] Dynamic property list of \"%s\" uses UID instead of original UID", GetParent()->GetFullName().c_str());
            return GetUID();
        }
    }
}
