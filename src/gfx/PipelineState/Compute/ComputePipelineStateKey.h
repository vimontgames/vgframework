#pragma once

#include "gfx/Shader/ComputeShaderKey.h"
#include "gfx/FrameGraph/RenderPassKey.h"

namespace vg::gfx
{
    struct ComputePipelineStateKey
    {
        ComputePipelineStateKey()
        {
            
        }

        RootSignatureHandle     m_computeRootSignature;
        ComputeShaderKey        m_computeShaderKey;
        RenderPassKey           m_computeRenderPassKey;
        core::u8                m_computeSubPassIndex = 0;

        bool operator == (const ComputePipelineStateKey & _other) const
        {
            return _other.m_computeRootSignature == m_computeRootSignature
                && _other.m_computeShaderKey     == m_computeShaderKey
                && _other.m_computeRenderPassKey == m_computeRenderPassKey
                && _other.m_computeSubPassIndex  == m_computeSubPassIndex;
        }

        bool operator != (const ComputePipelineStateKey & _other) const
        {
            return !(_other == *this);
        }

        struct hash
        {
            inline core::size_t operator() (const ComputePipelineStateKey & _this) const
            {
                return RootSignatureHandle::hash()(_this.m_computeRootSignature)
                     ^ ComputeShaderKey::hash()(_this.m_computeShaderKey)
                     ^ RenderPassKey::hash()(_this.m_computeRenderPassKey)
                     ^ core::hash<core::u8>()(_this.m_computeSubPassIndex);
            }
        };
    };
}