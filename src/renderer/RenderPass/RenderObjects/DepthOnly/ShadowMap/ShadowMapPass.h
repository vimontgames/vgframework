#pragma once

#include "renderer/RenderPass/RenderObjects/DepthOnly/DepthOnlyPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class ShadowMapPass final : public DepthOnlyPass
    {
    public:
        VG_CLASS_DECL(ShadowMapPass, DepthOnlyPass);

        ShadowMapPass();
        ~ShadowMapPass();

        void Setup(const gfx::RenderPassContext & _renderPassContext) final override;
        void Render(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) const final override;
    };
}