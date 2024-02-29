#pragma once

#include "renderer/RenderPass/RenderObjects/DepthOnly/DepthOnlyPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class DepthPrePass final : public DepthOnlyPass
    {
    public:
        const char * getClassName() const final { return "DepthPrepass"; }

        DepthPrePass();
        ~DepthPrePass();

        void Setup(const gfx::RenderPassContext & _renderPassContext) final override;
    };
}