#pragma once

#include "renderer/RenderPass/RenderObjects/RenderObjectsPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class DepthOnlyPass : public RenderObjectsPass
    {
    public:
        const char * getClassName() const final { return "DepthOnlyPass"; }

        DepthOnlyPass();
        ~DepthOnlyPass();

        void Setup(const gfx::RenderPassContext & _renderPassContext, double _dt) override;
        void Render(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) const override;
    };
}