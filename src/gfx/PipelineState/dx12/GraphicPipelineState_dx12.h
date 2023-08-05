#pragma once

#include "gfx/PipelineState/GraphicPipelineState.h"

namespace vg::gfx::dx12
{
    class GraphicPipelineState : public base::GraphicPipelineState
    {
        using super = base::GraphicPipelineState;

    public:
        GraphicPipelineState(const GraphicPipelineStateKey & _key);
        ~GraphicPipelineState();

        ID3D12PipelineState * getd3d12GraphicPipelineState() { return m_d3d12GraphicPipelineState; }

        static D3D12_PRIMITIVE_TOPOLOGY_TYPE getd3d12PrimitiveTopologyType(PrimitiveType _primitiveType);

    private:
        ID3D12PipelineState * m_d3d12GraphicPipelineState = nullptr;
    };
}