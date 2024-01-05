#pragma once

#include "renderer/RenderPass/RenderObjects/RenderObjectsPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class ForwardTransparentPass : public RenderObjectsPass
    {
    public:
        const char * getClassName() const final { return "ForwardTransparentPass"; }

        ForwardTransparentPass();
        ~ForwardTransparentPass();

        void Setup(const gfx::RenderPassContext & _renderPassContext, float _dt) override;
        void Render(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) const override;
    };
}