#pragma once
#include "Renderer/RenderPass/Render2D/Render2DPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    // This pass applies the HDROutput curve while copying the final image into backbuffer
    //--------------------------------------------------------------------------------------
    class HDROutputPass : public Render2DPass
    {
    public:
        const char * GetClassName() const final { return "HDROutputPass"; }

        HDROutputPass();
        ~HDROutputPass();

        void Setup(const gfx::RenderPassContext & _renderContext) override;
        void Render(const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) const override;

    private:
        gfx::RootSignatureHandle        m_hdrOutputRootSignature;
        gfx::ShaderKey                  m_hdrOutputCopyShaderKey;
        gfx::ShaderKey                  m_hdrOutputHDR10ShaderKey;
        gfx::ShaderKey                  m_hdrOutputHDR16ShaderKey;
    };
}