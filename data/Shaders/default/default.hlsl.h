#pragma once

#include "gfx/Shader/HLSLDesc.h"

namespace vg::gfx
{
    class DefaultHLSLDesc : public HLSLDesc
    {
    public:
        enum Flags : ShaderKey::Flags
        {
            Toolmode = 15
        };

        DefaultHLSLDesc()
        {
            setFile("default/default.hlsl");

            addFlag(Toolmode, ShaderStageFlags::VS | ShaderStageFlags::PS, "_TOOLMODE");

            auto & forward = addTechnique("Forward");
            {
                forward.vs = addVS("VS_Forward");
                forward.ps = addPS("PS_Forward");
            }
        }
    };
}