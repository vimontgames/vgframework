#pragma once

#include "gfx/Shader/HLSLDesc.h"

namespace vg::gfx
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

            auto & forward = addTechnique("Forward");
            {
                forward.vs = vsQuad;
                forward.ps = psQuad;
                forward.flags = (Flags)0;
            }

            auto psWire = addPS("PS_Wireframe");

            auto & wire = addTechnique("Wireframe");
            {
                wire.vs = vsQuad;
                wire.ps = psWire;
                wire.flags = (Flags)0;
            }
        }
    };
}