#pragma once

#include "renderer/RenderPass/RenderObjects/RenderObjectsPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class DeferredOpaquePass final : public RenderObjectsPass
    {
    public:
        VG_CLASS_DECL(DeferredOpaquePass, RenderObjectsPass);

        DeferredOpaquePass();
        ~DeferredOpaquePass();

        core::u64   GetCostEstimate     (const gfx::RenderPassContext & _renderPassContext) const final override;
        void        Setup               (const gfx::RenderPassContext & _renderPassContext) final override;
        void        Render              (const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) const final override;
    };
}