#pragma once

#include "Renderer/RenderPass/Update/UpdatePass.h"

namespace vg::gfx
{
    class Buffer;
}

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class MaterialDataUpdatePass : public UpdatePass
    {
    public:
        const char * GetClassName() const final { return "MaterialDataUpdatePass"; }

        MaterialDataUpdatePass();
        ~MaterialDataUpdatePass();

        void	        BeforeRender(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) final override;

    private:
        gfx::Buffer *   m_materialDataBuffer = nullptr;
    };
}