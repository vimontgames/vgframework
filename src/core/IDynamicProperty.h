#pragma once

#include "core/GameObject/GameObject.h"

namespace vg::core
{
    class IDynamicProperty : public Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IDynamicProperty, Object);

        virtual IProperty * GetProperty() const = 0;
        virtual bool Set(IObject * _object, IProperty * _prop) = 0;
    };
}