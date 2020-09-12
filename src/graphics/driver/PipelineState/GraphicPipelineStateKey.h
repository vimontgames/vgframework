#pragma once

#include "graphics/driver/RootSignature/RootSignature_consts.h"
#include "graphics/driver/PipelineState/PipelineState_consts.h"
#include "graphics/driver/PipelineState/RasterizerState.h"
#include "graphics/driver/Texture/Texture_consts.h"
#include "graphics/driver/Shader/ShaderKey.h"

namespace vg::graphics::driver
{
    enum class PrimitiveType : core::u8
    {
        Point = 0,
        Line,
        Triangle,
        Patch
    };

    struct GraphicPipelineStateKey
    {
        GraphicPipelineStateKey() :
            m_depthStencilFormat(PixelFormat::Unknow)
        {
            for (core::uint i = 0; i < maxRenderTarget; ++i)
                m_colorFormat[i] = PixelFormat::Unknow;
        }

        RootSignatureHandle     m_rootSignature;
        RasterizerState         m_rasterizerState;
        ShaderKey               m_shaderKey;
        PrimitiveType           m_primitiveType = PrimitiveType::Triangle;
        PixelFormat             m_colorFormat[maxRenderTarget];
        PixelFormat             m_depthStencilFormat;

        bool operator == (const GraphicPipelineStateKey & _other) const
        {
            return _other.m_rootSignature == m_rootSignature && _other.m_rasterizerState == m_rasterizerState && _other.m_shaderKey == m_shaderKey;
        }

        bool operator != (const GraphicPipelineStateKey & _other) const
        {
            return !(_other == *this);
        }

        struct hash
        {
            inline core::size_t operator() (const GraphicPipelineStateKey & _this) const
            {
                return RootSignatureHandle::hash()(_this.m_rootSignature)
                    ^ RasterizerState::hash()(_this.m_rasterizerState)
                    ^ ShaderKey::hash()(_this.m_shaderKey)
                    ^ core::hash<core::u8>()((core::u8)_this.m_primitiveType);
            }
        };
    };
}