#pragma once
#include "core/Options/Options.h"

namespace vg::gfx
{
    enum class VSync : core::u8;
    enum class HDR : core::u8;
    enum class AntiAliasing : core::u8;
}

namespace vg::renderer
{
    class IRendererOptions : public core::Options
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IRendererOptions, core::Options);

        virtual bool                IsToolModeEnabled   () const = 0;
        virtual bool                IsRayTracingEnabled () const = 0;

        virtual gfx::VSync          GetVSync            () const = 0;
        virtual bool                SetVSync            (const gfx::VSync & _vsync) = 0;

        virtual gfx::AntiAliasing   GetAliasing         () const = 0;
        virtual bool                SetAliasing         (const gfx::AntiAliasing & _aa) = 0;

        virtual gfx::HDR            GetHDR              () const = 0;
        virtual bool                SetHDR              (const gfx::HDR & _hdr) = 0;
    };
}