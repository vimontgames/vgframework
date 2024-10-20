#pragma once

#include "gfx/Shader/HLSLDesc.h"

namespace vg::gfx
{
    class BackgroundHLSLDesc : public HLSLDesc
    {
    public:
        BackgroundHLSLDesc()
        {
            enum Flags : ShaderKey::Flags
            {

            };

            setFile("background/background.hlsl");

            auto vsQuad = addVS("VS_Background");
            auto psQuad = addPS("PS_Gradient");

            auto & quad = addTechnique("Gradient");
            {
                quad.vs = vsQuad;
                quad.ps = psQuad;
                quad.flags = (Flags)0;
            }
        }
    };
}