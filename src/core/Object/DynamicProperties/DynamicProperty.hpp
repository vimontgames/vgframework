#include "DynamicProperty.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    bool DynamicProperty::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);  
        setPropertyFlag(DynamicProperty, m_name, PropertyFlags::NotVisible, true);
        setPropertyFlag(DynamicProperty, m_uid, PropertyFlags::NotVisible, true);
        setPropertyFlag(DynamicProperty, m_originalUID, PropertyFlags::NotVisible, true);
        setPropertyFlag(DynamicProperty, m_objectFlags, PropertyFlags::NotVisible, true);
        
        //registerProperty(DynamicProperty, m_enable, "Enable");
        
        return true;
    }
    
    //--------------------------------------------------------------------------------------
    DynamicProperty::DynamicProperty(const core::string & _name, core::IObject * _parent)
    {
        setName(_name);
        SetParent(_parent);
    }
    
    //--------------------------------------------------------------------------------------
    DynamicProperty::~DynamicProperty()
    {
    
    }

    //--------------------------------------------------------------------------------------
    bool DynamicProperty::RegisterUID()
    {
        return false;
    }

}