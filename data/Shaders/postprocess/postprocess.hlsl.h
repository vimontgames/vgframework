#pragma once

#include "gfx/Shader/HLSLDesc.h"

namespace vg::gfx
{
    class PostProcessHLSLDesc : public HLSLDesc
    {
    public:
        PostProcessHLSLDesc()
        {
            enum Flags : ShaderKey::Flags
            {

            };

            setFile("postprocess/postprocess.hlsl");

            auto & postProcessMainTechnique = addComputeTechnique("PostProcessMainCS");
            {
                postProcessMainTechnique.setComputeShader("CS_PostProcessMain");
            }
        }
    };
}