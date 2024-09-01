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
                Gamma = 0,
            };

            setFile("driver/driver.hlsl");

            addFlag(Gamma, ShaderStageFlags::PS, "_GAMMA");

            auto vsQuad = addVS("VS_Quad");
            auto psQuad = addPS("PS_Quad");

            auto & quad = addTechnique("Quad");
            {
                quad.vs = vsQuad;
                quad.ps = psQuad;
                quad.flags = (Flags)0;
            }

            auto psCopy = addPS("PS_Copy");

            auto & copy = addTechnique("Copy");
            {
                copy.vs = vsQuad;
                copy.ps = psCopy;
                copy.flags = (Flags)0;
            }

            auto & gamma = addTechnique("Gamma");
            {
                gamma.vs = vsQuad;
                gamma.ps = psCopy;
                gamma.addFlag(Flags::Gamma);
            }
        }
    };
}