#pragma once

#include "gfx/Shader/HLSLDesc.h"

namespace vg::gfx
{
    class DefaultHLSLDesc : public HLSLDesc
    {
    public:
        enum Flags : ShaderKey::Flags
        {
            // Flags 0..12 are available

            // Flags 13..15 are reserved and should be common for all shaders 
            ZOnly       = HLSLDesc::Flags::ZOnly,
            RayTracing  = HLSLDesc::Flags::RayTracing,
            Toolmode    = HLSLDesc::Flags::Toolmode
        };

        DefaultHLSLDesc()
        {
            setFile("default/default.hlsl");

            addFlag(ZOnly, ShaderStageFlags::VS | ShaderStageFlags::PS, "_ZONLY");
            addFlag(RayTracing, ShaderStageFlags::VS | ShaderStageFlags::PS, "_RAYTRACING");
            addFlag(Toolmode, ShaderStageFlags::VS | ShaderStageFlags::PS, "_TOOLMODE");

            auto & zonly = addTechnique("ZOnly");
            {
                zonly.vs = addVS("VS_Forward");
                zonly.ps = addPS("PS_Forward");
                zonly.addFlag(Flags::ZOnly);
            }

            auto & forward = addTechnique("Forward");
            {
                forward.vs = addVS("VS_Forward");
                forward.ps = addPS("PS_Forward");
            }

            auto & deferred = addTechnique("Deferred");
            {
                deferred.vs = addVS("VS_Deferred");
                deferred.ps = addPS("PS_Deferred");
            }
        }
    };
}