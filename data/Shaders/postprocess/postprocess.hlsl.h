#pragma once

#include "gfx/Shader/HLSLDesc.h"

namespace vg::gfx
{
    class PostProcessHLSLDesc : public HLSLDesc
    {
    public:
        enum Flags : ShaderKey::Flags
        {
            // Bits 0..1 for AA mode
            AAPostProcess   = 0,

            // Last flags are common for all shaders 
            RayTracing      = HLSLDesc::Flags::RayTracing,
            Toolmode        = HLSLDesc::Flags::Toolmode
        };

        PostProcessHLSLDesc()
        {
            setFile("postprocess/postprocess.hlsl");

            declFlags(AAPostProcess, ShaderStageFlags::CS, { "", "_FXAA", "_SMAA" } );
            declFlag(RayTracing, ShaderStageFlags::CS, "_RAYTRACING");
            declFlag(Toolmode, ShaderStageFlags::CS, "_TOOLMODE");

            auto csPostProcessMain = declCS("CS_PostProcessMain");

            auto & quad = declTechnique("PostProcessMainCS");
            {
                quad.cs = csPostProcessMain;
                quad.flags = (Flags)0;
            }
        }
    };
}