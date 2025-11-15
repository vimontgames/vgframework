#pragma once

#include "gfx/Shader/HLSLDesc.h"

namespace vg::gfx
{
    class DefaultHLSLDesc : public HLSLDesc
    {
    public:
        enum Flags : ShaderKey::Flags
        {
            // Last flags are common for all shaders 
            Particle    = HLSLDesc::Flags::Particle,
            Surface     = HLSLDesc::Flags::Surface,
            ZOnly       = HLSLDesc::Flags::ZOnly,
            RayTracing  = HLSLDesc::Flags::RayTracing,
            Toolmode    = HLSLDesc::Flags::Toolmode
        };

        DefaultHLSLDesc()
        {
            setFile("default/default.hlsl"); 

            declFlag(Particle, ShaderStageFlags::VS | ShaderStageFlags::PS, "_PARTICLE");
            declFlags(Surface, ShaderStageFlags::VS | ShaderStageFlags::PS, { "", "_ALPHATEST", "_ALPHABLEND", "_DECAL"});
            declFlag(ZOnly, ShaderStageFlags::VS | ShaderStageFlags::PS, "_ZONLY");
            declFlag(RayTracing, ShaderStageFlags::VS | ShaderStageFlags::PS, "_RAYTRACING");
            declFlag(Toolmode, ShaderStageFlags::VS | ShaderStageFlags::PS, "_TOOLMODE");

            auto & zonly = declTechnique("ZOnly");
            {
                zonly.vs = declVS("VS_Forward");
                zonly.ps = declPS("PS_Forward");
                zonly.setFlag(Flags::ZOnly);
            }

            auto & forward = declTechnique("Forward");
            {
                forward.vs = declVS("VS_Forward");
                forward.ps = declPS("PS_Forward");
            }

            auto & deferred = declTechnique("Deferred");
            {
                deferred.vs = declVS("VS_Deferred");
                deferred.ps = declPS("PS_Deferred");
            }

            auto & outline = declTechnique("Outline");
            {
                outline.vs = declVS("VS_Outline");
                outline.ps = declPS("PS_Outline");
            }
        }
    };
}