#include "DynamicPropertyUInt4.h"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(DynamicPropertyUInt4, "DynamicPropertyUInt4");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyUInt4::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(DynamicPropertyUInt4, m_value, "Value");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyUInt4::DynamicPropertyUInt4(const core::string & _name, core::IObject * _parent) :
        DynamicPropertyT<uint4>(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    core::uint4 * DynamicPropertyUInt4::GetPropertyPtr(const IObject * _object, const IProperty * _prop) const
    {
        return (core::uint4 *)_prop->GetPropertyUintN(_object, 4);
    }
}