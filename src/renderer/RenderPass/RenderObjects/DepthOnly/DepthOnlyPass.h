#pragma once
#include "renderer/RenderPass/RenderObjects/RenderObjectsPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class DepthOnlyPass : public RenderObjectsPass
    {
    public:
        VG_CLASS_DECL(DepthOnlyPass, RenderObjectsPass);

        DepthOnlyPass(const core::string & _name);
        ~DepthOnlyPass();

        core::u64   GetCostEstimate (const gfx::RenderPassContext & _renderPassContext) const override;
        void        Render          (const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) const override;
    };
}