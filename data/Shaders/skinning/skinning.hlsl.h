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

			auto csSkinningMain = declCS("CS_Skinning");

            auto & skinning = declTechnique("SkinningCS");
            {
                skinning.cs = declVS("CS_Skinning");
            }
        }
    };
}