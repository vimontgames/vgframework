#include "DynamicPropertyInt3.h"

namespace vg::core
{
    VG_REGISTER_CLASS(DynamicPropertyInt3, "DynamicPropertyInt3");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyInt3::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerOptionalProperty(DynamicPropertyInt3, m_enable, m_value, "Value");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyInt3::DynamicPropertyInt3(const core::string & _name, core::IObject * _parent) :
        DynamicPropertyT<int3>(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    core::int3 * DynamicPropertyInt3::GetPropertyPtr(const IObject * _object, const IProperty * _prop) const
    {
        return (core::int3 *)_prop->GetPropertyIntN(_object, 3);
    }
}