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

            addFlag(RayTracing, ShaderStageFlags::CS, "_RAYTRACING");
            addFlag(Toolmode, ShaderStageFlags::CS, "_TOOLMODE");

            auto & deferredLightingTech = addTechnique("DeferredLightingCS");
            {
                deferredLightingTech.cs = addCS("CS_DeferredLighting");
                deferredLightingTech.flags = (Flags)0;
            }
        }
    };
}