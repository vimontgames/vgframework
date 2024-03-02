#pragma once

#include "Renderer/RenderPass/Update/UpdatePass.h"

namespace vg::gfx
{
    class Buffer;
}

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class GPUDebugUpdatePass : public UpdatePass
    {
    public:
        const char * GetClassName() const final { return "GPUDebugUpdatePass"; }

        GPUDebugUpdatePass();
        ~GPUDebugUpdatePass();

        void	BeforeRender(const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) final override;

    private:
        gfx::Buffer * m_GPUDebugBuffer = nullptr;
        gfx::Buffer * m_GPUDebugBufferStaging = nullptr;
    };
}