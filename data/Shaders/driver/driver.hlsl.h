#pragma once

#include "graphics/driver/Shader/HLSLDesc.h"

namespace vg::graphics::driver
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

            auto vsQuad = addVS("VS_Quad");
            auto psQuad = addPS("PS_Quad");

            auto & quad = addTechnique("Quad");
            {
                quad.vs = vsQuad;
                quad.ps = psQuad;
                quad.flags = (Flags)0;
            }
        }
    };
}