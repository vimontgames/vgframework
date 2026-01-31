#include "DynamicPropertyUInt2.h"

namespace vg::core
{
    VG_REGISTER_CLASS(DynamicPropertyUInt2, "DynamicPropertyUInt2");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyUInt2::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerOptionalProperty(DynamicPropertyUInt2, m_enable, m_value, "Value");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyUInt2::DynamicPropertyUInt2(const core::string & _name, core::IObject * _parent) :
        DynamicPropertyT<uint2>(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    core::uint2 * DynamicPropertyUInt2::GetPropertyPtr(const IObject * _object, const IProperty * _prop) const
    {
        return (core::uint2 * )_prop->GetPropertyUintN(_object,2);
    }
}