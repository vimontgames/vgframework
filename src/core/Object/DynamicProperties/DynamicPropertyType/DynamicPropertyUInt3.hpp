#include "DynamicPropertyUInt3.h"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(DynamicPropertyUInt3, "DynamicPropertyUInt3");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyUInt3::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(DynamicPropertyUInt3, m_value, "Value");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyUInt3::DynamicPropertyUInt3(const core::string & _name, core::IObject * _parent) :
        DynamicPropertyT<uint3>(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    core::uint3 * DynamicPropertyUInt3::GetPropertyPtr(const IObject * _object, const IProperty * _prop) const
    {
        return (core::uint3 *)_prop->GetPropertyUintN(_object, 3);
    }
}