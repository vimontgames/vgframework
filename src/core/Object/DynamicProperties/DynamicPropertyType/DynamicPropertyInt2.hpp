#include "DynamicPropertyInt2.h"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(DynamicPropertyInt2, "DynamicPropertyInt2");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyInt2::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerOptionalProperty(DynamicPropertyInt2, m_enable, m_value, "Value");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyInt2::DynamicPropertyInt2(const core::string & _name, core::IObject * _parent) :
        DynamicPropertyT<int2>(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    core::int2 * DynamicPropertyInt2::GetPropertyPtr(const IObject * _object, const IProperty * _prop) const
    {
        return (core::int2 *)_prop->GetPropertyIntN(_object, 2);
    }
}