#include "DynamicPropertyFloat3.h"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(DynamicPropertyFloat3, "DynamicPropertyFloat3");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyFloat3::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        setPropertyFlag(DynamicPropertyFloat3, m_name, IProperty::Flags::NotVisible, false);
        registerProperty(DynamicPropertyFloat3, m_value, "Value");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyFloat3::DynamicPropertyFloat3(const core::string & _name, core::IObject * _parent) :
        DynamicPropertyT<float3>(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    core::float3 * DynamicPropertyFloat3::GetPropertyPtr(const IObject * _object, const IProperty * _prop) const
    {
        return _prop->GetPropertyFloat3(_object);
    }
}