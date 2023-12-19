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

            auto zOnlyFlag = declareFlag(ZOnly, "_ZONLY");
            auto toolmodeFlag = declareFlag(Toolmode, "_TOOLMODE");

            auto & zonlyTechnique = addGraphicsTechnique("ZOnly");
            {
                zonlyTechnique.setVertexShader("VS_Forward");
                zonlyTechnique.setPixelShader("PS_Forward");
                zonlyTechnique.setFlags(zOnlyFlag);
            }

            auto & forwardTechnique = addGraphicsTechnique("Forward");
            {
                forwardTechnique.setVertexShader("VS_Forward");
                forwardTechnique.setPixelShader("PS_Forward");
            }
        }
    };
}