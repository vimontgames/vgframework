#pragma once

#include "gfx/Shader/HLSLDesc.h"

namespace vg::gfx
{
    class DriverHLSLDesc : public HLSLDesc
    {
    public:
        enum Flags : ShaderKey::Flags
        {
            Gamma = 0,
            HDR = 1,  // 2 bits
            MSAA = 3   // 3 bits
        };

        DriverHLSLDesc()
        {
            setFile("driver/driver.hlsl");

            declFlag(Gamma, ShaderStageFlags::PS, "_GAMMA");
            declFlags(HDR, ShaderStageFlags::PS, { "", "_HDR10", "_HDR16" });
            declFlags(MSAA, ShaderStageFlags::PS, { "", "_MSAA2X", "_MSAA4X", "_MSAA8X", "_MSAA16X" });

            auto vsQuad = declVS("VS_Quad");
            auto psCopy = declPS("PS_Copy");

            auto & copy = declTechnique("Copy", vsQuad, psCopy);

            auto & gamma = declTechnique("Gamma", vsQuad, psCopy);
            gamma.setFlag(Flags::Gamma, true);

            auto & hdr10 = declTechnique("HDR10", vsQuad, psCopy);
            hdr10.setFlags(Flags::HDR, 1);

            auto & hdr16 = declTechnique("HDR16", vsQuad, psCopy);
            hdr16.setFlags(Flags::HDR, 2);

            auto & msaa2X = declTechnique("MSAA2X", vsQuad, psCopy);
            msaa2X.setFlags(MSAA, 1);

            auto & msaa4X = declTechnique("MSAA4X", vsQuad, psCopy);
            msaa4X.setFlags(MSAA, 2);

            auto & msaa8X = declTechnique("MSAA8X", vsQuad, psCopy);
            msaa8X.setFlags(MSAA, 3);

            auto & msaa16X = declTechnique("MSAA16X", vsQuad, psCopy);
            msaa16X.setFlags(MSAA, 4);
        }
    };
}