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

            declFlag(Gamma, ShaderStageFlags::PS, "_GAMMA");
            declFlag(HDR10, ShaderStageFlags::PS, "_HDR10");
            declFlag(HDR16, ShaderStageFlags::PS, "_HDR16");

            auto vsQuad = declVS("VS_Quad");
            auto psQuad = declPS("PS_Quad");
            auto psCopy = declPS("PS_Copy");

            auto & quad = declTechnique("Quad", vsQuad, psQuad);
            auto & copy = declTechnique("Copy", vsQuad, psCopy);
            auto & gamma = declTechnique("Gamma", vsQuad, psCopy, Flags::Gamma);
            auto & hdr10 = declTechnique("HDR10", vsQuad, psCopy, Flags::HDR10);
            auto & hdr16 = declTechnique("HDR16", vsQuad, psCopy, Flags::HDR16);
        }
    };
}