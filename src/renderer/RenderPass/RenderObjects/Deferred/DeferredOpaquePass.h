#pragma once

#include "renderer/RenderPass/RenderObjects/RenderObjectsPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class DeferredOpaquePass : public RenderObjectsPass
    {
    public:
        const char * GetClassName() const final { return "DeferredOpaquePass"; }

        DeferredOpaquePass();
        ~DeferredOpaquePass();

        void Setup(const gfx::RenderPassContext & _renderContext) override;
        void Render(const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) const override;
    };
}