#pragma once
#include "renderer/RenderPass/RenderObjects/RenderObjectsPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class DepthOnlyPass : public RenderObjectsPass
    {
    public:
        DepthOnlyPass(const core::string & _name);
        ~DepthOnlyPass();

        void Setup(const gfx::RenderPassContext & _renderPassContext, float _dt) = 0;
        void Render(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) const override;
    };
}