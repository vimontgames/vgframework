#pragma once

#include "gfx/Shader/HLSLDesc.h"

namespace vg::gfx
{
    class DefaultHLSLDesc : public HLSLDesc
    {
    public:
        //enum Flags : ShaderKey::Flags
        //{
        //    // Flags 0..12 are available
        //
        //    // Flags 13..15 are reserved and should be common for all shaders 
        //    ZOnly       = HLSLDesc::Flags::ZOnly,
        //    RayTracing  = HLSLDesc::Flags::RayTracing,
        //    Toolmode    = HLSLDesc::Flags::Toolmode
        //};

        DefaultHLSLDesc()
        {
            setFile("default/default.hlsl");

            declFlag(AlphaBlend, ShaderStageFlags::VS | ShaderStageFlags::PS, "_ALPHABLEND");
            declFlag(AlphaTest, ShaderStageFlags::VS | ShaderStageFlags::PS, "_ALPHATEST");
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
        }
    };
}