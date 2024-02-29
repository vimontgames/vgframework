#pragma once

#include "renderer/RenderPass/RenderObjects/DepthOnly/DepthOnlyPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class ShadowMapPass final : public DepthOnlyPass
    {
    public:
        const char * getClassName() const final { return "ShadowMapPass"; }

        ShadowMapPass();
        ~ShadowMapPass();

        void Setup(const gfx::RenderPassContext & _renderPassContext) final override;
    };
}