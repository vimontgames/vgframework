#include "DynamicPropertyU32.h"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(DynamicPropertyU32, "DynamicPropertyU32");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyU32::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerOptionalProperty(DynamicPropertyU32, m_enable, m_value, "Value");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyU32::DynamicPropertyU32(const core::string & _name, core::IObject * _parent) :
        DynamicPropertyT<u32>(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    u32 * DynamicPropertyU32::GetPropertyPtr(const IObject * _object, const IProperty * _prop) const
    {
        return _prop->GetPropertyUint32(_object);
    }
}