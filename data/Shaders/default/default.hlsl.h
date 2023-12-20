#pragma once

#include "gfx/Shader/HLSLDesc.h"

namespace vg::gfx
{
    class DefaultHLSLDesc : public HLSLDesc
    {
    public:
        enum Flags : ShaderKey::Flags
        {
            // Flags 0..13 are available

            // Flags 14..15 are reserved and should be common for all shaders             
            ZOnly       = 14,
            Toolmode    = 15
        };

        DefaultHLSLDesc()
        {
            setFile("default/default.hlsl");

            addFlag(ZOnly, ShaderStageFlags::VS | ShaderStageFlags::PS, "_ZONLY");
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
        }
    };
}