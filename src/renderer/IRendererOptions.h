#pragma once
#include "core/Options/Options.h"

namespace vg::renderer
{
    class IRendererOptions : public core::Options
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IRendererOptions, core::Options);

        virtual bool    IsToolModeEnabled   () const = 0;
        virtual bool    IsRayTracingEnabled () const = 0;
    };
}