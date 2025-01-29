#pragma once

#include "gfx/Shader/HLSLDesc.h"

namespace vg::gfx
{
    class DebugDrawHLSLDesc : public HLSLDesc
    {
    public:
        enum Flags : ShaderKey::Flags
        {
            Toolmode = HLSLDesc::Flags::Toolmode
        };

        DebugDrawHLSLDesc()
        {
            setFile("debugdraw/debugdraw.hlsl");

            declFlag(Toolmode, ShaderStageFlags::VS | ShaderStageFlags::PS, "_TOOLMODE");

            auto & debugDraw = declTechnique("DebugDraw");
            {
                debugDraw.vs = declVS("VS_DebugDraw");
                debugDraw.ps = declPS("PS_DebugDraw");

                debugDraw.setFlag(Flags::Toolmode);
            }
        }
    };
}