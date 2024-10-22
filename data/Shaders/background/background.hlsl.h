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

            auto vsQuad = declVS("VS_Background");
            auto psQuad = declPS("PS_Gradient");

            auto & quad = declTechnique("Gradient");
            {
                quad.vs = vsQuad;
                quad.ps = psQuad;
                quad.flags = (Flags)0;
            }
        }
    };
}