#include "DynamicPropertyFloat4.h"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(DynamicPropertyFloat4, "DynamicPropertyFloat4");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyFloat4::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        setPropertyFlag(GameObject, m_name, IProperty::Flags::NotVisible, false);
        registerProperty(DynamicPropertyFloat4, m_value, "Value");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyFloat4::DynamicPropertyFloat4(const core::string & _name, core::IObject * _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyFloat4::DynamicPropertyFloat4(const core::IObject * _object, const core::IProperty * _prop)
    {
        setName(_prop->getName());
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyFloat4::~DynamicPropertyFloat4()
    {

    }

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyFloat4::Set(IObject * _object, IProperty * _prop)
    {
        if (float4 * value = _prop->GetPropertyFloat4(_object))
        {
            *value = m_value;
            return true;
        }
        return false;
    }
}