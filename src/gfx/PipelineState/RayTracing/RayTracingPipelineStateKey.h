#pragma once

#include "gfx/RootSignature/RootSignature_consts.h"
#include "gfx/Shader/RayTracingShaderKey.h"

namespace vg::gfx
{
    struct RayTracingPipelineStateKey
    {
        RayTracingPipelineStateKey()
        {

        }

        bool operator == (const RayTracingPipelineStateKey & _other) const
        {
            return _other.m_rayTracingRootSignature == m_rayTracingRootSignature
                && _other.m_rayTracingShaderKey == m_rayTracingShaderKey;
        }

        bool operator != (const RayTracingPipelineStateKey & _other) const
        {
            return !(_other == *this);
        }

        struct hash
        {
            inline core::size_t operator() (const RayTracingPipelineStateKey & _this) const
            {
                return RootSignatureHandle::hash()(_this.m_rayTracingRootSignature)
                    ^ RayTracingShaderKey::hash()(_this.m_rayTracingShaderKey);
            }
        };

        RootSignatureHandle     m_rayTracingRootSignature;
        RayTracingShaderKey     m_rayTracingShaderKey;
    };
}