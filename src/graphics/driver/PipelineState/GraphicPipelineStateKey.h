#pragma once

#include "graphics/driver/RootSignature/RootSignature_consts.h"
#include "graphics/driver/PipelineState/PipelineState_consts.h"
#include "graphics/driver/PipelineState/RasterizerState.h"
#include "graphics/driver/Resource/Texture_consts.h"
#include "graphics/driver/Shader/ShaderKey.h"
#include "graphics/driver/FrameGraph/RenderPassKey.h"

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
        GraphicPipelineStateKey() 
        {
           
        }

        RootSignatureHandle     m_rootSignature;
        RasterizerState         m_rasterizerState;
        ShaderKey               m_shaderKey;
        RenderPassKey           m_renderPassKey;

        core::u8                m_subPassIndex = 0;
        PrimitiveType           m_primitiveType = PrimitiveType::Triangle;

        bool operator == (const GraphicPipelineStateKey & _other) const
        {
            return _other.m_rootSignature == m_rootSignature && _other.m_rasterizerState == m_rasterizerState && _other.m_shaderKey == m_shaderKey && _other.m_renderPassKey == m_renderPassKey;
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
                    ^ RenderPassKey::hash()(_this.m_renderPassKey)
                    ^ core::hash<core::u8>()(_this.m_subPassIndex)
                    ^ core::hash<std::underlying_type<PrimitiveType>::type>()(std::underlying_type<PrimitiveType>::type(_this.m_primitiveType));
            }
        };
    };
}