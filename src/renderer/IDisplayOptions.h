#pragma once
#include "core/Object/Object.h"

namespace vg::renderer
{
    class IDisplayOptions : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IDisplayOptions, core::Object);

        virtual bool IsCollidersEnabled() const = 0;
    };
}