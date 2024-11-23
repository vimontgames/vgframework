#pragma once

#include "gfx/Shader/HLSLDesc.h"

namespace vg::gfx
{
    class DriverHLSLDesc : public HLSLDesc
    {
        enum Flags : ShaderKey::Flags
        {
            Gamma   = 0,    // 1 bit  (ON/OFF)
            HDR     = 1,    // 2 bits (HDR enum)
            MSAA    = 3,    // 3 bits (MSAA enum)
            Depth   = 6     // 1 bit  (ON/OFF)
        };

    public:
        DriverHLSLDesc()
        {
            setFile("driver/driver.hlsl");

            declFlag(Gamma, ShaderStageFlags::PS, "_GAMMA");
            declFlags(HDR, ShaderStageFlags::PS, { "", "_HDR10", "_HDR16" });
            declFlags(MSAA, ShaderStageFlags::PS, { "", "_MSAA2X", "_MSAA4X", "_MSAA8X", "_MSAA16X" });
            declFlag(Depth, ShaderStageFlags::PS, "_DEPTH");

            auto vsQuad = declVS("VS_Quad");
            auto psCopy = declPS("PS_Copy");

            auto & copy = declTechnique("Copy", vsQuad, psCopy);

            auto & gamma = declTechnique("Gamma", vsQuad, psCopy);
            gamma.setFlag(Gamma, true);

            auto & hdr10 = declTechnique("HDR10", vsQuad, psCopy);
            hdr10.setValue(HDR, HDR::HDR10);

            auto & hdr16 = declTechnique("HDR16", vsQuad, psCopy);
            hdr16.setValue(HDR, HDR::HDR16);

            auto & resolveColorMSAA2X = declTechnique("ResolveColorMSAA2X", vsQuad, psCopy);
            resolveColorMSAA2X.setValue(MSAA, MSAA::MSAA2X);

            auto & resolveColorMSAA4X = declTechnique("ResolveColorMSAA4X", vsQuad, psCopy);
            resolveColorMSAA4X.setValue(MSAA, MSAA::MSAA4X);

            auto & resolveColorMSAA8X = declTechnique("ResolveColorMSAA8X", vsQuad, psCopy);
            resolveColorMSAA8X.setValue(MSAA, MSAA::MSAA8X);

            auto & resolveColorMSAA16X = declTechnique("ResolveColorMSAA16X", vsQuad, psCopy);
            resolveColorMSAA16X.setValue(MSAA, MSAA::MSAA16X);

            auto psCopyLinearDepth = declPS("PS_CopyLinearDepth");

            auto & copyLinearDepth = declTechnique("CopyLinearDepth", vsQuad, psCopyLinearDepth);

            auto & resolveLinearDepthMSAA2X = declTechnique("ResolveLinearDepthMSAA2X", vsQuad, psCopyLinearDepth);
            resolveLinearDepthMSAA2X.setValue(MSAA, MSAA::MSAA2X);

            auto & resolveLinearDepthMSAA4X = declTechnique("ResolveLinearDepthMSAA4X", vsQuad, psCopyLinearDepth);
            resolveLinearDepthMSAA4X.setValue(MSAA, MSAA::MSAA4X);

            auto & resolveLinearDepthMSAA8X = declTechnique("ResolveLinearDepthMSAA8X", vsQuad, psCopyLinearDepth);
            resolveLinearDepthMSAA8X.setValue(MSAA, MSAA::MSAA8X);

            auto & resolveLinearDepthMSAA16X = declTechnique("ResolveLinearDepthMSAA16X", vsQuad, psCopyLinearDepth);
            resolveLinearDepthMSAA16X.setValue(MSAA, MSAA::MSAA16X);
        }
    };
}