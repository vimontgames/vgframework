#include "DynamicPropertyString.h"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(DynamicPropertyString, "DynamicPropertyString");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyString::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        setPropertyFlag(GameObject, m_name, IProperty::Flags::NotVisible, false);
        registerProperty(DynamicPropertyString, m_value, "Value");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyString::DynamicPropertyString(const core::string & _name, core::IObject * _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyString::DynamicPropertyString(const core::IObject * _object, const core::IProperty * _prop)
    {
        setName(_prop->getName());
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyString::~DynamicPropertyString()
    {

    }
}