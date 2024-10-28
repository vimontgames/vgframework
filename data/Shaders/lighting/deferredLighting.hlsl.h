#pragma once

#include "gfx/Shader/HLSLDesc.h"

namespace vg::gfx
{
    class DeferredLightingHLSLDesc : public HLSLDesc
    {
    public:
        enum Flags : ShaderKey::Flags
        {
            // 3 bits for MSAA
            MSAA = 0,

            // Reserved 11..15
            RayTracing = HLSLDesc::Flags::RayTracing,
            Toolmode = HLSLDesc::Flags::Toolmode
        };

        DeferredLightingHLSLDesc()
        {
            setFile("lighting/deferredLighting.hlsl");

            declFlags(MSAA, ShaderStageFlags::CS, { "", "_MSAA2X", "_MSAA4X", "_MSAA8X", "_MSAA16X" });
            declFlag(RayTracing, ShaderStageFlags::CS, "_RAYTRACING");
            declFlag(Toolmode, ShaderStageFlags::CS, "_TOOLMODE");

            auto & deferredLightingTech = declTechnique("DeferredLightingCS");
            {
                deferredLightingTech.cs = declCS("CS_DeferredLighting");
                deferredLightingTech.flags = (Flags)0;
            }
        }
    };
}