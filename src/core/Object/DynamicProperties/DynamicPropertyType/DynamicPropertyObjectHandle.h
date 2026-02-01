#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"
#include "core/Object/ObjectHandle.h"

namespace vg::core
{
    class DynamicPropertyObjectHandle : public DynamicPropertyT<ObjectHandle>
    {
    public:
        VG_CLASS_DECL(DynamicPropertyObjectHandle, DynamicPropertyT);

        DynamicPropertyObjectHandle(const core::string & _name = "", core::IObject * _parent = nullptr);

        ObjectHandle * GetPropertyPtr(const IObject * _object, const IProperty * _prop) const final override;
    };
}