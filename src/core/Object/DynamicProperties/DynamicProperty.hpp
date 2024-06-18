#include "DynamicProperty.h"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(DynamicProperty, "DynamicProperty");

    //--------------------------------------------------------------------------------------
    bool DynamicProperty::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);    
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
}