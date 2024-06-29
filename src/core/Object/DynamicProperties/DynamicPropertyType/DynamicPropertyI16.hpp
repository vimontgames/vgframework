#include "DynamicPropertyI16.h"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(DynamicPropertyI16, "DynamicPropertyI16");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyI16::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        setPropertyFlag(DynamicPropertyI16, m_name, IProperty::Flags::NotVisible, false);
        registerProperty(DynamicPropertyI16, m_value, "Value");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyI16::DynamicPropertyI16(const core::string & _name, core::IObject * _parent) :
        DynamicPropertyT<i16>(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    i16 * DynamicPropertyI16::GetPropertyPtr(const IObject * _object, const IProperty * _prop) const
    {
        return _prop->GetPropertyInt16(_object);
    }
}