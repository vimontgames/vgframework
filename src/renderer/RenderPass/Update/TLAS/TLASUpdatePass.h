#pragma once

#include "Renderer/RenderPass/Update/UpdatePass.h"

namespace vg::gfx
{
    class Buffer;
}

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class TLASUpdatePass : public UpdatePass
    {
    public:
        const char * GetClassName() const final { return "TLASUpdatePass"; }

        TLASUpdatePass();
        ~TLASUpdatePass();

        void    BeforeAll(const gfx::RenderPassContext & _renderPassContext) final override;
        void	Setup(const gfx::RenderPassContext & _renderPassContext) final override;
        void	BeforeRender(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) final override;
    };
}