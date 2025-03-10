#pragma once
#include "Renderer/RenderPass/Render2D/Render2DPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    // This pass copies the final image to its editor viewport or backbuffer in fullscreen mode
    //--------------------------------------------------------------------------------------
    class FinalBlitPass : public Render2DPass
    {
    public:
        const char * GetClassName() const final { return "FinalBlitPass"; }

        FinalBlitPass();
        ~FinalBlitPass();

        void Setup(const gfx::RenderPassContext & _renderContext) override;
        void Render(const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) const override;

    private:
        gfx::RootSignatureHandle        m_postProcessRootSignature;
        gfx::ShaderKey                  m_postProcessShaderKeyCopy;
        gfx::ShaderKey                  m_postProcessShaderKeyGamma;
    };
}