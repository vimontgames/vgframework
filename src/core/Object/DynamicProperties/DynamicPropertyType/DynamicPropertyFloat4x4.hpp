#include "DynamicPropertyFloat4x4.h"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(DynamicPropertyFloat4x4, "DynamicPropertyFloat4x4");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyFloat4x4::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerOptionalProperty(DynamicPropertyFloat4x4, m_enable, m_value, "Value");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyFloat4x4::DynamicPropertyFloat4x4(const core::string & _name, core::IObject * _parent) :
        DynamicPropertyT<float4x4>(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    core::float4x4 * DynamicPropertyFloat4x4::GetPropertyPtr(const IObject * _object, const IProperty * _prop) const
    {
        return _prop->GetPropertyFloat4x4(_object);
    }
}