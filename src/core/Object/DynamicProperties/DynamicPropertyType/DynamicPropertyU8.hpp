#include "DynamicPropertyU8.h"

namespace vg::core
{
    VG_REGISTER_CLASS(DynamicPropertyU8, "DynamicPropertyU8");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyU8::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerOptionalProperty(DynamicPropertyU8, m_enable, m_value, "Value");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyU8::DynamicPropertyU8(const core::string & _name, core::IObject * _parent) :
        DynamicPropertyT<u8>(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    u8 * DynamicPropertyU8::GetPropertyPtr(const IObject * _object, const IProperty * _prop) const
    {
        return _prop->GetPropertyUint8(_object);
    }
}