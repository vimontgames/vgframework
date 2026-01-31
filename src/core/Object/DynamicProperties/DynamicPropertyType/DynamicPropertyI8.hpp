#include "DynamicPropertyI8.h"

namespace vg::core
{
    VG_REGISTER_CLASS(DynamicPropertyI8, "DynamicPropertyI8");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyI8::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerOptionalProperty(DynamicPropertyI8, m_enable, m_value, "Value");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyI8::DynamicPropertyI8(const core::string & _name, core::IObject * _parent) :
        DynamicPropertyT<i8>(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    i8 * DynamicPropertyI8::GetPropertyPtr(const IObject * _object, const IProperty * _prop) const
    {
        return _prop->GetPropertyInt8(_object);
    }
}