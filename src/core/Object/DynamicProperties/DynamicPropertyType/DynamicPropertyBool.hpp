#include "DynamicPropertyBool.h"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(DynamicPropertyBool, "DynamicPropertyBool");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyBool::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(DynamicPropertyBool, m_value, "Value");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyBool::DynamicPropertyBool(const core::string & _name, core::IObject * _parent) :
        DynamicPropertyT<bool>(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    bool * DynamicPropertyBool::GetPropertyPtr(const IObject * _object, const IProperty * _prop) const
    {
        return _prop->GetPropertyBool(_object);
    }
}