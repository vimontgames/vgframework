#pragma once

#include "core/Object/Object.h"
#include "GraphicPipelineState_consts.h"
#include "GraphicPipelineStateKey.h"

namespace vg::gfx
{
    namespace base
    {
        class GraphicPipelineState : public core::Object
        {
        public:
            GraphicPipelineState(const GraphicPipelineStateKey & _key);
            ~GraphicPipelineState();

            const GraphicPipelineStateKey & getGraphicPipelineStateKey() const { return m_key; }

        private:
            GraphicPipelineStateKey m_key;
        };
    }
}

#ifdef VG_DX12
#include "dx12/GraphicPipelineState_dx12.h"
#elif defined(VG_VULKAN)
#include "vulkan/GraphicPipelineState_vulkan.h"
#else
#error Undefined GFXAPI
#endif

namespace vg::gfx
{
    class GraphicPipelineState : public VG_GFXAPI::GraphicPipelineState
    {
        using super = VG_GFXAPI::GraphicPipelineState;

    public:
        const char * GetClassName() const final { return "GraphicPipelineState"; }

        GraphicPipelineState(const GraphicPipelineStateKey & _key);
        ~GraphicPipelineState();

        static GraphicPipelineState * createGraphicPipelineState(const GraphicPipelineStateKey & _key);
    };
}