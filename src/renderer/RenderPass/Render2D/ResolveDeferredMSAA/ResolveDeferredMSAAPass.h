#pragma once

#include "Renderer/RenderPass/Render2D/Render2DPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class ResolveDeferredMSAAPass : public Render2DPass
    {
    public:
        const char * GetClassName() const final { return "ResolveDeferredMSAAPass"; }

        ResolveDeferredMSAAPass();
        ~ResolveDeferredMSAAPass();

        void Setup(const gfx::RenderPassContext & _renderContext) override;
        void Render(const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) const override;

    private:
        gfx::RootSignatureHandle        m_resolveDeferredMSAARootSignature;
        gfx::ShaderKey                  m_resolveDeferredMSAA2XShaderKey;
        gfx::ShaderKey                  m_resolveDeferredMSAA4XShaderKey;
        gfx::ShaderKey                  m_resolveDeferredMSAA8XShaderKey;
        gfx::ShaderKey                  m_resolveDeferredMSAA16XShaderKey;
    };
}