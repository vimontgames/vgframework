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

            auto & debugDraw = addTechnique("DebugDraw");
            {
                debugDraw.vs = addVS("VS_DebugDraw");
                debugDraw.ps = addPS("PS_DebugDraw");
            }
        }
    };
}