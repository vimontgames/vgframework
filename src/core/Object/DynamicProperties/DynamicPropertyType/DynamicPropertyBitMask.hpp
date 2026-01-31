#include "DynamicPropertyBitMask.h"

namespace vg::core
{
    VG_REGISTER_CLASS(DynamicPropertyBitMask, "DynamicPropertyBitMask");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyBitMask::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerOptionalProperty(DynamicPropertyBitMask, m_enable, m_value, "Value");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyBitMask::DynamicPropertyBitMask(const core::string & _name, core::IObject * _parent) :
        DynamicPropertyT<BitMask>(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    BitMask * DynamicPropertyBitMask::GetPropertyPtr(const IObject * _object, const IProperty * _prop) const
    {
        return _prop->GetPropertyBitMask(_object);
    }
}