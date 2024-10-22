#pragma once

#include "gfx/Shader/HLSLDesc.h"

namespace vg::gfx
{
    class DeferredLightingHLSLDesc : public HLSLDesc
    {
    public:
        DeferredLightingHLSLDesc()
        {
            enum Flags : ShaderKey::Flags
            {
                RayTracing = HLSLDesc::Flags::RayTracing,
                Toolmode = HLSLDesc::Flags::Toolmode
            };

            setFile("lighting/deferredLighting.hlsl");

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