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
    const IDynamicProperty * DynamicPropertyList::GetProperty(const IProperty * _prop) const
    {
        for (uint i = 0; i < m_properties.size(); ++i)
        {
            auto & prop = m_properties[i];
            if (prop->getName() == _prop->getName())
            {
                //memcpy(&prop->m_prop, _prop, sizeof(Property));

                prop->m_prop = *(Property*)_prop;
                prop->m_prop.setOffset(offsetof(DynamicPropertyString, m_value));
                return prop;
            }
        }

        return nullptr;
    }
}
