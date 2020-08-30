#pragma once

#include "graphics/driver/PipelineState/GraphicPipelineState.h"

namespace vg::graphics::driver::vulkan
{
    class GraphicPipelineState : public base::GraphicPipelineState
    {
        using super = base::GraphicPipelineState;

    public:
        GraphicPipelineState();
        ~GraphicPipelineState();
    };
}