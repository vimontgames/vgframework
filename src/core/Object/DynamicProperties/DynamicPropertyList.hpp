#include "DynamicPropertyList.h"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(DynamicPropertyList, "DynamicPropertyList");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyList::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        //registerPropertyObjectPtrVectorEx(DynamicPropertyList, m_properties, "Properties", IProperty::Flags::Flatten); // "Flatten" is not implemented for ObjectPtrVector
        registerPropertyObjectPtrVector(DynamicPropertyList, m_properties, "Properties");
        setPropertyFlag(DynamicPropertyList, m_uid, IProperty::Flags::NotVisible | IProperty::Flags::Debug, false);

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

        return nullptr;
    }
}
