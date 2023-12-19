#pragma once

#include "gfx/Shader/HLSLDesc.h"

namespace vg::gfx
{
    class SkinningHLSLDesc : public HLSLDesc
    {
    public:
        enum Flags : ShaderKey::Flags
        {

        };

        SkinningHLSLDesc()
        {
            setFile("skinning/skinning.hlsl");

            auto & skinning = addComputeTechnique("SkinningCS");
            {
                skinning.setComputeShader("CS_Skinning");
            }
        }
    };
}