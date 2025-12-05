#pragma once

#include "Renderer/RenderPass/Update/UpdatePass.h"
#include "core/Container/Span.h"

namespace vg::gfx
{
    class Buffer;
}

namespace vg::renderer
{
    class GraphicInstance;

    //--------------------------------------------------------------------------------------
    class InstanceDataUpdatePass final : public UpdatePass
    {
    public:
        const char * GetClassName() const final { return "InstanceDataUpdatePass"; }

                                        InstanceDataUpdatePass();
                                        ~InstanceDataUpdatePass();
        
        core::u64                       GetCostEstimate         (const gfx::RenderPassContext & _renderContext) const final override;
        void                            BeforeAll               (const gfx::RenderPassContext & _renderContext) final override;
        void                            BeforeRender            (const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) final override;

        core::span<GraphicInstance *>   getGraphicInstances     () const;

    private:
        gfx::Buffer *                   m_instanceDataBuffer = nullptr;
        core::uint                      m_mapSize = 0;
    };
}