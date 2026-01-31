#include "DynamicPropertyInt4.h"

namespace vg::core
{
    VG_REGISTER_CLASS(DynamicPropertyInt4, "DynamicPropertyInt4");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyInt4::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerOptionalProperty(DynamicPropertyInt4, m_enable, m_value, "Value");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyInt4::DynamicPropertyInt4(const core::string & _name, core::IObject * _parent) :
        DynamicPropertyT<int4>(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    core::int4 * DynamicPropertyInt4::GetPropertyPtr(const IObject * _object, const IProperty * _prop) const
    {
        return (core::int4 *)_prop->GetPropertyIntN(_object, 4);
    }
}