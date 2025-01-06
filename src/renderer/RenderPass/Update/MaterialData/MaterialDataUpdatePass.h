#pragma once

#include "Renderer/RenderPass/Update/UpdatePass.h"

namespace vg::gfx
{
    class Buffer;
}

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class MaterialDataUpdatePass final : public UpdatePass
    {
    public:
        const char * GetClassName() const final { return "MaterialDataUpdatePass"; }

        MaterialDataUpdatePass();
        ~MaterialDataUpdatePass();

        void BeforeAll(const gfx::RenderPassContext & _renderContext) final override;
        void BeforeRender(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) final override;

    private:
        gfx::Buffer *                   m_materialDataBuffer = nullptr;
        core::vector<MaterialModel *>   m_materials;
    };
}