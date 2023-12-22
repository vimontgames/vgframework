#pragma once

#include "gfx/Shader/HLSLDesc.h"

namespace vg::gfx
{
    class PostProcessHLSLDesc : public HLSLDesc
    {
    public:
        PostProcessHLSLDesc()
        {
            enum Flags : ShaderKey::Flags
            {
                // Flags 13..15 are reserved and should be common for all shaders 
                ZOnly       = HLSLDesc::Flags::ZOnly,
                RayTracing  = HLSLDesc::Flags::RayTracing,
                Toolmode    = HLSLDesc::Flags::Toolmode
            };

            setFile("postprocess/postprocess.hlsl");

            addFlag(RayTracing, ShaderStageFlags::CS, "_RAYTRACING");
            addFlag(Toolmode, ShaderStageFlags::CS, "_TOOLMODE");

            auto csPostProcessMain = addCS("CS_PostProcessMain");

            auto & quad = addTechnique("PostProcessMainCS");
            {
                quad.cs = csPostProcessMain;
                quad.flags = (Flags)0;
            }
        }
    };
}