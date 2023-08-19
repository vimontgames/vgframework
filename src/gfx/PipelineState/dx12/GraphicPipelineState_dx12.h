#pragma once

#include "gfx/PipelineState/GraphicPipelineState.h"

namespace vg::gfx
{
    class GraphicPipelineState;

    namespace dx12
    {
        class GraphicPipelineState : public base::GraphicPipelineState
        {
            using super = base::GraphicPipelineState;

        public:
            GraphicPipelineState(const GraphicPipelineStateKey & _key);
            ~GraphicPipelineState();

            VG_INLINE ID3D12PipelineState * getd3d12GraphicPipelineState() const;

            static gfx::GraphicPipelineState * createGraphicPipelineState(const GraphicPipelineStateKey & _key);
            VG_INLINE static D3D12_PRIMITIVE_TOPOLOGY_TYPE getd3d12PrimitiveTopologyType(PrimitiveType _primitiveType);

        private:
            VG_INLINE void setd3d12GraphicPipelineState(ID3D12PipelineState * _d3d12GraphicPipelineState);
            static bool created3d12GraphicPipelineState(const GraphicPipelineStateKey & _key, ID3D12PipelineState *& _d3d12GraphicPipelineState);

        private:
            ID3D12PipelineState * m_d3d12GraphicPipelineState = nullptr;
        };
    }
}

#if VG_ENABLE_INLINE
#include "GraphicPipelineState_dx12.inl"
#endif