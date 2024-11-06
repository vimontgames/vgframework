#pragma once

#include "gfx/Shader/HLSLDesc.h"

namespace vg::gfx
{
    class PreviewHLSLDesc : public HLSLDesc
    {
    public:
        enum Flags : ShaderKey::Flags
        {
            Type = 0
        };

        PreviewHLSLDesc()
        {
            setFile("preview/preview.hlsl");

            declFlags(Type, ShaderStageFlags::PS, { "", "_TEX2D", "_TEXCUBE" });

            auto vsPreview = declVS("VS_Preview");
            auto psPreview = declPS("PS_Preview");

            auto & preview = declTechnique("Preview", vsPreview, psPreview);
        }
    };
}