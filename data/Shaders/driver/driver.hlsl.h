#pragma once

#include "gfx/Shader/HLSLDesc.h"

namespace vg::gfx
{
    class DriverHLSLDesc : public HLSLDesc
    {
    public:
        DriverHLSLDesc()
        {
            enum Flags : ShaderKey::Flags
            {

            };

            setFile("driver/driver.hlsl");

            auto & quadTechnique = addGraphicsTechnique("Quad");
            {
                quadTechnique.setVertexShader("VS_Quad");
                quadTechnique.setPixelShader("PS_Quad");
            }

            auto & copyTechnique = addGraphicsTechnique("Copy");
            {
                copyTechnique.setVertexShader("VS_Quad");
                copyTechnique.setPixelShader("PS_Copy");
            }

            auto & gamma = addGraphicsTechnique("Gamma");
            {
                gamma.setVertexShader("VS_Quad");
                gamma.setPixelShader("PS_Gamma");
            }
        }
    };
}