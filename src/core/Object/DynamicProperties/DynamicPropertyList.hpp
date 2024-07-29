#include "DynamicPropertyList.h"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(DynamicPropertyList, "DynamicPropertyList");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyList::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        //registerProperty(DynamicPropertyList, m_uid, "UID");
        //registerProperty(DynamicPropertyList, m_name, "Name");

        registerPropertyObjectPtrVector(DynamicPropertyList, m_properties, "Properties");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyList::DynamicPropertyList(const core::string & _name, core::IObject * _parent)
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
            if (prop->getName() == _prop->getName())
            {
                prop->CopyProperty(_prop);
                return prop;
            }
        }

        //for (uint i = 0; i < m_properties.size(); ++i)
        //{
        //    const auto & prop = m_properties[i];
        //
        //    if (prop->getType() == IProperty::Type::Resource)
        //    {
        //
        //    }
        //}

        return nullptr;
    }
}
