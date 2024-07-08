#include "DynamicPropertyI64.h"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(DynamicPropertyI64, "DynamicPropertyI64");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyI64::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(DynamicPropertyI64, m_value, "Value");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyI64::DynamicPropertyI64(const core::string & _name, core::IObject * _parent) :
        DynamicPropertyT<i64>(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    i64 * DynamicPropertyI64::GetPropertyPtr(const IObject * _object, const IProperty * _prop) const
    {
        return _prop->GetPropertyInt64(_object);
    }
}