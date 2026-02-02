#pragma once

#include "gfx/Shader/HLSLDesc.h"

namespace vg::gfx
{
    class ScreenSpaceAmbientHLSLDesc : public HLSLDesc
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

        ScreenSpaceAmbientHLSLDesc()
        {
            setFile("lighting/screenSpaceAmbient.hlsl");

            declFlags(MSAA, ShaderStageFlags::CS, { "", "_MSAA2X", "_MSAA4X", "_MSAA8X", "_MSAA16X" });
            declFlag(RayTracing, ShaderStageFlags::CS, "_RAYTRACING");
            declFlag(Toolmode, ShaderStageFlags::CS, "_TOOLMODE");

            auto & screenSpaceAmbientTech = declTechnique("ScreenSpaceAmbient");
            {
                screenSpaceAmbientTech.cs = declCS("CS_ScreenSpaceAmbient");
                screenSpaceAmbientTech.flags = (Flags)0;
            }
        }
    };
}