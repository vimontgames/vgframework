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

            auto csPostProcessMain = addCS("CS_PostProcessMain");

            auto & quad = addTechnique("PostProcessMainCS");
            {
                quad.cs = csPostProcessMain;
                quad.flags = (Flags)0;
            }
        }
    };
}