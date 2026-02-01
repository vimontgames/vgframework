#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class IModel : public Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IModel, Object);
    };
}