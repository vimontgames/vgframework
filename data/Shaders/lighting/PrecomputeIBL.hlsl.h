#pragma once

#include "gfx/Shader/HLSLDesc.h"

namespace vg::gfx
{
    class PrecomputeIBLHLSLDesc : public HLSLDesc
    {
    public:
        enum Flags : ShaderKey::Flags
        {
            Toolmode = HLSLDesc::Flags::Toolmode
        };

        PrecomputeIBLHLSLDesc()
        {
            setFile("lighting/PrecomputeIBL.hlsl");

            declFlag(Toolmode, ShaderStageFlags::CS, "_TOOLMODE");

            auto & generateSpecularBRDFTech = declTechnique("GenerateSpecularBRDF");
            {
                generateSpecularBRDFTech.cs = declCS("CS_GenerateSpecularBRDF");
                generateSpecularBRDFTech.flags = (Flags)0;
            }
        }
    };
}