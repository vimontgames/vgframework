#include "DynamicProperty.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    bool DynamicProperty::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);  
        registerProperty(DynamicProperty, m_enable, "Enable");
        return true;
    }
    
    //--------------------------------------------------------------------------------------
    DynamicProperty::DynamicProperty(const core::string & _name, core::IObject * _parent)
    {
    
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