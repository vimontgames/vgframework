#include "DynamicPropertyFloat4.h"

namespace vg::core
{
    VG_REGISTER_CLASS(DynamicPropertyFloat4, "DynamicPropertyFloat4");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyFloat4::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerOptionalProperty(DynamicPropertyFloat4, m_enable, m_value, "Value");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyFloat4::DynamicPropertyFloat4(const core::string & _name, core::IObject * _parent) :
        DynamicPropertyT<float4>(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    core::float4 * DynamicPropertyFloat4::GetPropertyPtr(const IObject * _object, const IProperty * _prop) const
    {
        return _prop->GetPropertyFloat4(_object);
    }
}