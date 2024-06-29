#include "DynamicPropertyU16.h"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(DynamicPropertyU16, "DynamicPropertyU16");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyU16::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        setPropertyFlag(DynamicPropertyU16, m_name, IProperty::Flags::NotVisible, false);
        registerProperty(DynamicPropertyU16, m_value, "Value");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyU16::DynamicPropertyU16(const core::string & _name, core::IObject * _parent) :
        DynamicPropertyT<u16>(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    u16 * DynamicPropertyU16::GetPropertyPtr(const IObject * _object, const IProperty * _prop) const
    {
        return _prop->GetPropertyUint16(_object);
    }
}