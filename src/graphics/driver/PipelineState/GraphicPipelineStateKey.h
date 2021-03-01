#pragma once

#include "graphics/driver/RootSignature/RootSignature_consts.h"
#include "graphics/driver/PipelineState/PipelineState_consts.h"
#include "graphics/driver/PipelineState/RasterizerState.h"
#include "graphics/driver/PipelineState/DepthStencilState.h"
#include "graphics/driver/PipelineState/BlendState.h"
#include "graphics/driver/Resource/Texture_consts.h"
#include "graphics/driver/Shader/ShaderKey.h"
#include "graphics/driver/FrameGraph/RenderPassKey.h"

namespace vg::graphics::driver
{
    struct GraphicPipelineStateKey
    {
        GraphicPipelineStateKey() 
        {
           
        }

        RootSignatureHandle     m_rootSignature;
        RasterizerState         m_rasterizerState;
        DepthStencilState       m_depthStencilState;
        BlendState              m_blendState;
        ShaderKey               m_shaderKey;
        RenderPassKey           m_renderPassKey;

        core::u8                m_subPassIndex = 0;
        PrimitiveTopology       m_primitiveTopology = PrimitiveTopology::TriangleList;

        PrimitiveType getPrimitiveType() const;

        bool operator == (const GraphicPipelineStateKey & _other) const
        {
            return _other.m_rootSignature == m_rootSignature 
                && _other.m_rasterizerState == m_rasterizerState 
                && _other.m_depthStencilState == m_depthStencilState
                && _other.m_blendState == m_blendState 
                && _other.m_shaderKey == m_shaderKey 
                && _other.m_renderPassKey == m_renderPassKey
                && _other.m_primitiveTopology == m_primitiveTopology;
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
                    ^ DepthStencilState::hash()(_this.m_depthStencilState)
                    ^ BlendState::hash()(_this.m_blendState)
                    ^ ShaderKey::hash()(_this.m_shaderKey)
                    ^ RenderPassKey::hash()(_this.m_renderPassKey)
                    ^ core::hash<core::u8>()(_this.m_subPassIndex)
                    ^ core::hash<std::underlying_type<PrimitiveTopology>::type>()(std::underlying_type<PrimitiveTopology>::type(_this.m_primitiveTopology));
            }
        };
    };
}