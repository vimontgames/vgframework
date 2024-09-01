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
                HDR10,
                HDR16
            };

            setFile("driver/driver.hlsl");

            addFlag(Gamma, ShaderStageFlags::PS, "_GAMMA");
            addFlag(HDR10, ShaderStageFlags::PS, "_HDR10");
            addFlag(HDR16, ShaderStageFlags::PS, "_HDR16");

            auto vsQuad = addVS("VS_Quad");
            auto psQuad = addPS("PS_Quad");
            auto psCopy = addPS("PS_Copy");

            auto & quad = addTechnique("Quad", vsQuad, psQuad);
            auto & copy = addTechnique("Copy", vsQuad, psCopy);
            auto & gamma = addTechnique("Gamma", vsQuad, psCopy, Flags::Gamma);
            auto & hdr10 = addTechnique("HDR10", vsQuad, psCopy, Flags::HDR10);
            auto & hdr16 = addTechnique("HDR16", vsQuad, psCopy, Flags::HDR16);
        }
    };
}