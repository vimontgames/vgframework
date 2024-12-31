#pragma once

#include "Renderer/RenderPass/Update/UpdatePass.h"

namespace vg::gfx
{
    class Buffer;
}

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class InstanceDataUpdatePass final : public UpdatePass
    {
    public:
        const char * GetClassName() const final { return "InstanceDataUpdatePass"; }

        InstanceDataUpdatePass();
        ~InstanceDataUpdatePass();

        void Prepare(const gfx::RenderPassContext & _renderContext) final override;
        void BeforeRender(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) final override;

    private:
        gfx::Buffer * m_instanceDataBuffer = nullptr;
        core::uint m_mapSize = 0;
    };
}