#pragma once

#include "renderer/RenderPass/RenderObjects/RenderObjectsPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class OutlineMaskPass final : public RenderObjectsPass
    {
        public:
            VG_CLASS_DECL(OutlineMaskPass, RenderObjectsPass);

            OutlineMaskPass();
            ~OutlineMaskPass();

            core::u64   GetCostEstimate(const gfx::RenderPassContext & _renderPassContext) const final override;
            void        Setup(const gfx::RenderPassContext & _renderPassContext) final override;
            void        Render(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) const final override;
    };
}