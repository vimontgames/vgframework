#include "DynamicPropertyFloat2.h"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(DynamicPropertyFloat2, "DynamicPropertyFloat2");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyFloat2::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        setPropertyFlag(DynamicPropertyFloat2, m_name, IProperty::Flags::NotVisible, false);
        registerProperty(DynamicPropertyFloat2, m_value, "Value");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyFloat2::DynamicPropertyFloat2(const core::string & _name, core::IObject * _parent) :
        DynamicPropertyT<float2>(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    core::float2 * DynamicPropertyFloat2::GetPropertyPtr(const IObject * _object, const IProperty * _prop) const
    {
        return _prop->GetPropertyFloat2(_object);
    }
}