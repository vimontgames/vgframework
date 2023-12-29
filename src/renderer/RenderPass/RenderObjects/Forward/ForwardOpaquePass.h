#pragma once

#include "renderer/RenderPass/RenderObjects/RenderObjectsPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class ForwardOpaquePass : public RenderObjectsPass
    {
    public:
        const char * getClassName() const final { return "ForwardPass"; }

        ForwardOpaquePass();
        ~ForwardOpaquePass();

        void Setup(const gfx::RenderPassContext & _renderContext, float _dt) override;
        void Render(const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) const override;
    };
}