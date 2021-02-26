#pragma once

#include "graphics/driver/Shader/HLSLDesc.h"

namespace vg::graphics::driver
{
    class DefaultHLSLDesc : public HLSLDesc
    {
    public:
        DefaultHLSLDesc()
        {
            enum Flags : ShaderKey::Flags
            {

            };

            setFile("default/default.hlsl");

            auto vsQuad = addVS("VS_Forward");
            auto psQuad = addPS("PS_Forward");

            auto & quad = addTechnique("Forward");
            {
                quad.vs = vsQuad;
                quad.ps = psQuad;
                quad.flags = (Flags)0;
            }
        }
    };
}