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

        void Setup(const gfx::RenderPassContext & _renderContext, double _dt) override;
        void Render(const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) const override;
    };
}