#include "DynamicPropertyU64.h"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(DynamicPropertyU64, "DynamicPropertyU64");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyU64::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerOptionalProperty(DynamicPropertyU64, m_enable, m_value, "Value");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyU64::DynamicPropertyU64(const core::string & _name, core::IObject * _parent) :
        DynamicPropertyT<u64>(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    u32 * DynamicPropertyU64::GetPropertyPtr(const IObject * _object, const IProperty * _prop) const
    {
        return _prop->GetPropertyUint64(_object);
    }
}