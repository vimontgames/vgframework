#pragma once

#include "gfx/Shader/HLSLDesc.h"

namespace vg::gfx
{
    class DebugDrawHLSLDesc : public HLSLDesc
    {
    public:
        DebugDrawHLSLDesc()
        {
            setFile("debugdraw/debugdraw.hlsl");

            auto & debugDraw = declTechnique("DebugDraw");
            {
                debugDraw.vs = declVS("VS_DebugDraw");
                debugDraw.ps = declPS("PS_DebugDraw");
            }
        }
    };
}