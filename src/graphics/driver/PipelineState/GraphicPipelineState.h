#pragma once

#include "core/Object/Object.h"
#include "PipelineState_consts.h"
#include "GraphicPipelineStateKey.h"

namespace vg::graphics::driver
{
    namespace base
    {
        class GraphicPipelineState : public core::Object
        {
        public:
            GraphicPipelineState(const GraphicPipelineStateKey & _key);
            ~GraphicPipelineState();

        private:
            GraphicPipelineStateKey m_key;
        };
    }
}

#include VG_GFXAPI_HEADER(GraphicPipelineState)

namespace vg::graphics::driver
{
    class GraphicPipelineState : public VG_GFXAPI::GraphicPipelineState
    {
        using super = VG_GFXAPI::GraphicPipelineState;

    public:
        GraphicPipelineState(const GraphicPipelineStateKey & _key);
        ~GraphicPipelineState();
    };
}