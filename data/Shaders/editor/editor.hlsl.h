#pragma once

#include "gfx/Shader/HLSLDesc.h"

namespace vg::gfx
{
    class EditorHLSLDesc : public HLSLDesc
    {
    public:
        EditorHLSLDesc()
        {
            setFile("editor/editor.hlsl");

            auto & debugDraw = addTechnique("DebugDraw");
            {
                debugDraw.vs = addVS("VS_DebugDraw");
                debugDraw.ps = addPS("PS_DebugDraw");
            }
        }
    };
}