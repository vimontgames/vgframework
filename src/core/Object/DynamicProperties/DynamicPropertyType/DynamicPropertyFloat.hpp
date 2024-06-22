#include "DynamicPropertyFloat.h"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(DynamicPropertyFloat, "DynamicPropertyFloat");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyFloat::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        setPropertyFlag(GameObject, m_name, IProperty::Flags::NotVisible, false);
        registerProperty(DynamicPropertyFloat, m_value, "Value");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyFloat::DynamicPropertyFloat(const core::string & _name, core::IObject * _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyFloat::DynamicPropertyFloat(const core::IObject * _object, const core::IProperty * _prop)
    {
        setName(_prop->getName());
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyFloat::~DynamicPropertyFloat()
    {

    }

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyFloat::Set(IObject * _object, IProperty * _prop)
    {
        if (float * value = _prop->GetPropertyFloat(_object))
        {
            *value = m_value;
            return true;
        }
        return false;
    }
}