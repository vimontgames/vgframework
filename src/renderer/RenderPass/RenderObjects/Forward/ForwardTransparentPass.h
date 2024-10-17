#pragma once

#include "renderer/RenderPass/RenderObjects/RenderObjectsPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class ForwardTransparentPass : public RenderObjectsPass
    {
    public:
        VG_CLASS_DECL(ForwardTransparentPass, RenderObjectsPass);

        ForwardTransparentPass();
        ~ForwardTransparentPass();

        void Setup(const gfx::RenderPassContext & _renderPassContext) override;
        void Render(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) const override;


    };
}