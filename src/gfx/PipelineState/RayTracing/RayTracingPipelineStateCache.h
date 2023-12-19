#pragma once

#include "gfx/PipelineState/PipelineStateCache.h"
#include "gfx/PipelineState/RayTracing/RayTracingPipelineStateKey.h"

namespace vg::gfx
{
    class RayTracingPipelineState;

    class RayTracingPipelineStateCache : public PipelineStateCache
    {
    public:
        using super = PipelineStateCache;
        using RayTracingPipelineStateHash = core::unordered_map<gfx::RayTracingPipelineStateKey, gfx::RayTracingPipelineState *, gfx::RayTracingPipelineStateKey::hash>;

        RayTracingPipelineStateCache() = default;

        void clear();
        void reset();
        void resetShaders(ComputeShaderKey::File _file);

        bool getRayTracingPipelineState(const RayTracingPipelineStateKey & _key, RayTracingPipelineState *& _rayTracingPipelineState);

        RayTracingPipelineStateKey     m_rayTracingPipelineKey;
        RayTracingPipelineStateHash    m_rayTracingPipelineStateHash;
    };
}