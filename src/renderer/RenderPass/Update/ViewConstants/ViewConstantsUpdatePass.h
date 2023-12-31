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

        void	Setup(const gfx::RenderPassContext & _renderPassContext, float _dt) final override;
        void	BeforeRender(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) final override;

    protected:
        void    updateLightsConstants(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList);

    private:
        static gfx::Buffer * s_ViewConstantsBuffer;
        static gfx::Buffer * s_LightsConstantsBuffer;
    };
}