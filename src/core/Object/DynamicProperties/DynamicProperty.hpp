#include "DynamicProperty.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    bool DynamicProperty::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc); 

        setPropertyFlag(DynamicProperty, m_name, PropertyFlags::Hidden, true);
        setPropertyFlag(DynamicProperty, m_originalUID, PropertyFlags::Hidden, true);
        setPropertyFlag(DynamicProperty, m_objectFlags, PropertyFlags::Hidden, true);
        
        return true;
    }
    
    //--------------------------------------------------------------------------------------
    DynamicProperty::DynamicProperty(const core::string & _name, core::IObject * _parent)
    {
        SetName(_name);
        SetParent(_parent);
        RegisterUID();
    }
    
    //--------------------------------------------------------------------------------------
    DynamicProperty::~DynamicProperty()
    {
        UnregisterUID();
    }

    //--------------------------------------------------------------------------------------
    bool DynamicProperty::RegisterUID()
    {
        return super::RegisterUID();
    }
}