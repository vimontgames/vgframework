#include "DynamicPropertyObjectHandle.h"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(DynamicPropertyObjectHandle, "DynamicPropertyObjectHandle");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyObjectHandle::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerOptionalProperty(DynamicPropertyObjectHandle, m_enable, m_value, "Value");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyObjectHandle::DynamicPropertyObjectHandle(const core::string & _name, core::IObject * _parent) :
        DynamicPropertyT<ObjectHandle>(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    ObjectHandle * DynamicPropertyObjectHandle::GetPropertyPtr(const IObject * _object, const IProperty * _prop) const
    {
        return _prop->GetPropertyObjectHandle(_object);
    }
}