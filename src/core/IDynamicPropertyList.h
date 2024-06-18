#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class IDynamicPropertyList : public Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IDynamicPropertyList, Object);

        virtual const IDynamicProperty * GetProperty(const IProperty * _prop) const = 0;
    };
}