#pragma once

#include "gfx/PipelineState/PipelineStateCache.h"
#include "gfx/PipelineState/Compute/ComputePipelineStateKey.h"

namespace vg::gfx
{
    class ComputePipelineState;

    class ComputePipelineStateCache : public PipelineStateCache
    {
    public:
        using super = PipelineStateCache;
        using ComputePipelineStateHash = core::unordered_map<gfx::ComputePipelineStateKey, gfx::ComputePipelineState *, gfx::ComputePipelineStateKey::hash>;

        ComputePipelineStateCache() = default;

        void clear();
        void reset();
        void resetComputePipelineStates(ComputeShaderKey::File _file);
        static void resetComputePipelineStatesImpl(ComputePipelineStateHash & _computePipelineStateHash, ComputeShaderKey::File _file);

        bool getComputePipelineState(const ComputePipelineStateKey & _key, ComputePipelineState *& _computePipelineState);

        ComputePipelineStateKey     m_computePipelineKey;
        ComputePipelineStateHash    m_computePipelineStateHash;
    };
}