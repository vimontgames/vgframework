#include "DynamicProperty.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    bool DynamicProperty::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);  
        setPropertyFlag(DynamicProperty, m_name, IProperty::Flags::NotVisible, true);
        setPropertyFlag(DynamicProperty, m_originalUID, IProperty::Flags::NotVisible, true);
        setPropertyFlag(DynamicProperty, m_objectFlags, IProperty::Flags::NotVisible, true);
        
        //registerProperty(DynamicProperty, m_enable, "Enable");
        
        return true;
    }
    
    //--------------------------------------------------------------------------------------
    DynamicProperty::DynamicProperty(const core::string & _name, core::IObject * _parent)
    {
        setName(_name);
        setParent(_parent);
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