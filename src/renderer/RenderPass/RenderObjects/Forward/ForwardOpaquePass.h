#pragma once

#include "renderer/RenderPass/RenderObjects/RenderObjectsPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class ForwardOpaquePass : public RenderObjectsPass
    {
    public:
        const char * GetClassName() const final { return "ForwardOpaquePass"; }

        ForwardOpaquePass();
        ~ForwardOpaquePass();

        void Setup(const gfx::RenderPassContext & _renderPassContext) override;
        void Render(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) const override;
    };
}