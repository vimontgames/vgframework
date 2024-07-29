#include "DynamicPropertyString.h"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(DynamicPropertyString, "DynamicPropertyString");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyString::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(DynamicPropertyString, m_value, "Value");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyString::DynamicPropertyString(const core::string & _name, core::IObject * _parent) :
        DynamicPropertyT<string>(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    string * DynamicPropertyString::GetPropertyPtr(const IObject * _object, const IProperty * _prop) const
    {
        return _prop->GetPropertyString(_object);
    }
}