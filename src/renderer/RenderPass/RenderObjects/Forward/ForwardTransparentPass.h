#pragma once

#include "renderer/RenderPass/RenderObjects/RenderObjectsPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class ForwardTransparentPass final : public RenderObjectsPass
    {
    public:
        VG_CLASS_DECL(ForwardTransparentPass, RenderObjectsPass);

        ForwardTransparentPass();
        ~ForwardTransparentPass();

        void Setup(const gfx::RenderPassContext & _renderPassContext) final override;
        void BeforeRender(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) final override;
        void Render(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) const final override;

    private:
        static gfx::Buffer * s_TransparentPassConstantsBuffer;
    };
}