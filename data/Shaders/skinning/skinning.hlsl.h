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

			auto csSkinningMain = addCS("CS_Skinning");

            auto & skinning = addTechnique("SkinningCS");
            {
                skinning.cs = addVS("CS_Skinning");
            }
        }
    };
}