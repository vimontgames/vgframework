#pragma once
#include "core/Options/Options.h"

namespace vg::gfx
{
    enum class MSAA : core::u8;
    enum class HDR : core::u8;
    enum class AAPostProcess : core::u8;
    enum class VSync : core::u8;
    class ITexture;
}

namespace vg::renderer
{
    class IRendererOptions : public core::Options
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IRendererOptions, core::Options);

        virtual const core::float4 &    GetDefaultClearColor        () const = 0;
        virtual gfx::ITexture *         GetDefaultCubemap           () const = 0;
        virtual float                   GetDefaultAmbientIntensity  () const = 0;

        virtual bool                    IsToolModeEnabled           () const = 0;
        virtual bool                    IsRayTracingEnabled         () const = 0;

        virtual gfx::MSAA               GetMSAA                     () const = 0;
        virtual bool                    SetMSAA                     (gfx::MSAA _msaa) = 0;

        virtual gfx::HDR                GetHDR                      () const = 0;
        virtual bool                    SetHDR                      (gfx::HDR _hdr) = 0;


        virtual gfx::AAPostProcess      GetAAPostProcess            () const = 0;
        virtual bool                    SetAAPostProcess            (gfx::AAPostProcess _aa) = 0;

        virtual gfx::VSync              GetVSync                    () const = 0;
        virtual bool                    SetVSync                    (gfx::VSync _vsync) = 0;
    };
}