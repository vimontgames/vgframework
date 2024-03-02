#pragma once

#include "Renderer/RenderPass/Update/UpdatePass.h"

namespace vg::gfx
{
    class Buffer;
}

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class BLASUpdatePass : public UpdatePass
    {
    public:
        const char * GetClassName() const final { return "RTASUpdatePass"; }

        BLASUpdatePass();
        ~BLASUpdatePass();

        void	Setup(const gfx::RenderPassContext & _renderPassContext) final override;
        void	BeforeRender(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) final override;
    };
}