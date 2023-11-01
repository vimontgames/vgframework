#pragma once

#include "Renderer/RenderPass/Update/UpdatePass.h"

namespace vg::gfx
{
    class Buffer;
}

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class ViewConstantsUpdatePass : public UpdatePass
    {
    public:
        const char * getClassName() const final { return "ViewConstantsUpdatePass"; }

        ViewConstantsUpdatePass();
        ~ViewConstantsUpdatePass();

        void	BeforeRender(const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) final override;

    private:
        static gfx::Buffer * s_ViewConstantsBuffer;
    };
}