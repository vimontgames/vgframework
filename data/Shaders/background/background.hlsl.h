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

            auto & gradientTechnique = addGraphicsTechnique("Gradient");
            {
                gradientTechnique.setVertexShader("VS_Background");
                gradientTechnique.setPixelShader("PS_Gradient");
            }
        }
    };
}