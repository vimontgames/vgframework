#pragma once

#include "gfx/Shader/HLSLDesc.h"

namespace vg::gfx
{
    class PostProcessHLSLDesc : public HLSLDesc
    {
    public:
        enum Flags : ShaderKey::Flags
        {
            AAPostProcess   = 0,    // Bits 0..1 for AA mode
            MSAA            = 2,    // Bits 2..4 for MSAA
            Pass            = 5, 

            // Last flags are common for all shaders 
            RayTracing      = HLSLDesc::Flags::RayTracing,
            Toolmode        = HLSLDesc::Flags::Toolmode
        };

        PostProcessHLSLDesc()
        {
            setFile("postprocess/postprocess.hlsl");

            declFlags(MSAA, ShaderStageFlags::CS, { "", "_MSAA2X", "_MSAA4X", "_MSAA8X", "_MSAA16X" });
            declFlags(AAPostProcess, ShaderStageFlags::CS, { "", "_FXAA", "_SMAA" } );
            declFlags(Pass, ShaderStageFlags::CS, { "_PASS0", "_PASS1" });

            declFlag(RayTracing, ShaderStageFlags::CS, "_RAYTRACING");
            declFlag(Toolmode, ShaderStageFlags::CS, "_TOOLMODE");

            auto csResolveMSAA = declCS("CS_ResolveMSAA");
            auto & resolveMSAA = declTechnique("ResolveMSAACS");
            {
                resolveMSAA.cs = csResolveMSAA;
                resolveMSAA.flags = (Flags)0;
            }

            auto csDepthOfField = declCS("CS_DepthOfField");
            auto & depthOfField = declTechnique("DepthOfFieldCS");
            {
                depthOfField.cs = csDepthOfField;
                depthOfField.flags = (Flags)0;
            }

            auto csPostProcessMain = declCS("CS_PostProcessMain");
            auto & postProcessMain = declTechnique("PostProcessMainCS");
            {
                postProcessMain.cs = csPostProcessMain;
                postProcessMain.flags = (Flags)0;
            }
        }
    };
}