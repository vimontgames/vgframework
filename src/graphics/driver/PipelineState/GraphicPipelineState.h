#pragma once

#include "core/Object/Object.h"
#include "PipelineState_consts.h"

namespace vg::graphics::driver
{
    namespace base
    {
        class GraphicPipelineState : public core::Object
        {
        public:
            GraphicPipelineState();
            ~GraphicPipelineState();
        };
    }
}

#include VG_GRAPHICSAPI_HEADER(GraphicPipelineState)

namespace vg::graphics::driver
{
    class GraphicPipelineState : public VG_GRAPHICSAPI::GraphicPipelineState
    {
        using super = VG_GRAPHICSAPI::GraphicPipelineState;

    public:
        GraphicPipelineState();
        ~GraphicPipelineState();
    };
}