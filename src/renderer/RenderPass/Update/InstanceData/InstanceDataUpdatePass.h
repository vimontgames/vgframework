#pragma once

#include "Renderer/RenderPass/Update/UpdatePass.h"

namespace vg::gfx
{
    class Buffer;
}

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class InstanceDataUpdatePass : public UpdatePass
    {
    public:
        const char * getClassName() const final { return "InstanceDataUpdatePass"; }

        InstanceDataUpdatePass();
        ~InstanceDataUpdatePass();

        void	BeforeRender(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) final override;

    private:
        gfx::Buffer * m_InstanceDataConstantsBuffer = nullptr;
    };
}