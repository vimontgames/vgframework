#pragma once

#include "gfx/PipelineState/PipelineStateCache.h"
#include "gfx/PipelineState/Graphic/GraphicPipelineStateKey.h"

namespace vg::gfx
{
    class GraphicPipelineState;
    class Buffer;

    class GraphicPipelineStateCache : public PipelineStateCache
    {
    public:
        using super = PipelineStateCache;
        using GraphicPipelineStateHash = core::unordered_map<gfx::GraphicPipelineStateKey, gfx::GraphicPipelineState *, gfx::GraphicPipelineStateKey::hash>;

        GraphicPipelineStateCache();

        void clear();
        void reset();
        void resetGraphicPipelineStates(ShaderKey::File _file);
        static void resetGraphicPipelineStatesImpl(GraphicPipelineStateHash & _graphicPipelineStateHash, ShaderKey::File _file);

        bool getGraphicPipelineState(const GraphicPipelineStateKey & _key, GraphicPipelineState *& _graphicPipelineState);

        GraphicPipelineStateKey     m_graphicPipelineKey;
        core::uint4                 m_viewport;
        core::uint4                 m_scissor;
        Buffer *                    m_indexBuffer = nullptr;
        core::u8                    m_stencilRef = 0x0;
        GraphicPipelineStateHash    m_graphicPipelineStateHash;
    };
}