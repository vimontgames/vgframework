#include "DynamicPropertyI32.h"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(DynamicPropertyI32, "DynamicPropertyI32");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyI32::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerOptionalProperty(DynamicPropertyI32, m_enable, m_value, "Value");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyI32::DynamicPropertyI32(const core::string & _name, core::IObject * _parent) :
        DynamicPropertyT<i32>(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    i32 * DynamicPropertyI32::GetPropertyPtr(const IObject * _object, const IProperty * _prop) const
    {
        return _prop->GetPropertyInt32(_object);
    }
}