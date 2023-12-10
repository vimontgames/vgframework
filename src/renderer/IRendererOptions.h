#pragma once
#include "core/Object/Object.h"

namespace vg::renderer
{
    class IRendererOptions : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IRendererOptions, core::Object);
    };
}