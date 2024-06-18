#pragma once

#include "core/GameObject/GameObject.h"

namespace vg::core
{
    class IDynamicProperty : public Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IDynamicProperty, Object);

        virtual IProperty * GetProperty() const = 0;
    };
}