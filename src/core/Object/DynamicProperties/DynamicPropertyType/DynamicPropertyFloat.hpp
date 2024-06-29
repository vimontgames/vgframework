#include "DynamicPropertyFloat.h"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(DynamicPropertyFloat, "DynamicPropertyFloat");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyFloat::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        setPropertyFlag(DynamicPropertyFloat, m_name, IProperty::Flags::NotVisible, false);
        registerProperty(DynamicPropertyFloat, m_value, "Value");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyFloat::DynamicPropertyFloat(const core::string & _name, core::IObject * _parent) :
        DynamicPropertyT<float>(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    float * DynamicPropertyFloat::GetPropertyPtr(const IObject * _object, const IProperty * _prop) const
    {
        return _prop->GetPropertyFloat(_object);
    }
}