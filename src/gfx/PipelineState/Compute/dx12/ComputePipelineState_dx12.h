#pragma once

#include "gfx/PipelineState/Compute/ComputePipelineState.h"

namespace vg::gfx
{
    class ComputePipelineState;

    namespace dx12
    {
        class ComputePipelineState : public base::ComputePipelineState
        {
            using super = base::ComputePipelineState;

        public:
            ComputePipelineState(const ComputePipelineStateKey & _key);
            ~ComputePipelineState();

            VG_INLINE ID3D12PipelineState * getd3d12ComputePipelineState() const;

            static gfx::ComputePipelineState * createComputePipelineState(const ComputePipelineStateKey & _computeKey);

        private:
            VG_INLINE void setd3d12ComputePipelineState(ID3D12PipelineState * _d3d12ComputePipelineState);
            static bool created3d12ComputePipelineState(const ComputePipelineStateKey & _computeKey, ID3D12PipelineState *& _d3d12ComputePipelineState);

        private:
            ID3D12PipelineState * m_d3d12ComputePipelineState = nullptr;
        };
    }
}

#if VG_ENABLE_INLINE
#include "ComputePipelineState_dx12.inl"
#endif