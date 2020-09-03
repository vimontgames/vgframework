#pragma once

#include "graphics/driver/PipelineState/GraphicPipelineState.h"

namespace vg::graphics::driver::dx12
{
    class GraphicPipelineState : public base::GraphicPipelineState
    {
        using super = base::GraphicPipelineState;

    public:
        GraphicPipelineState();
        ~GraphicPipelineState();

    private:
        ID3D12PipelineState * m_d3d12GraphicPipelineState = nullptr;
    };
}