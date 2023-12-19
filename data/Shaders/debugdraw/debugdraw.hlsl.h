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

            auto & debugDraw = addGraphicsTechnique("DebugDraw");
            {
                debugDraw.setVertexShader("VS_DebugDraw");
                debugDraw.setPixelShader("PS_DebugDraw");
            }
        }
    };
}