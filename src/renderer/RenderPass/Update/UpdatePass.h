#pragma once

#include "renderer/RenderPass/RenderPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class UpdatePass : public RenderPass
    {
    public:
        UpdatePass(const core::string & _passName);
        ~UpdatePass();

        void	Setup           (const gfx::RenderPassContext & _renderContext, float _dt) override {}
        void	BeforeRender    (const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) override = 0;
        void	AfterRender     (const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) override {};
        void	Render          (const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) const override {}
    };
}