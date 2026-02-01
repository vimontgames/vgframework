#include "DynamicProperty.h"

namespace vg::core
{
    VG_REGISTER_TEMPLATE_CLASS(DynamicPropertyT, core::string, "Dynamic Property Template");
    VG_REGISTER_ABSTRACT_CLASS(DynamicProperty);

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