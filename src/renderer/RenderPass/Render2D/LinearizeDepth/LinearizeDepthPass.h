#pragma once

#include "Renderer/RenderPass/Render2D/Render2DPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class LinearizeDepthPass : public Render2DPass
    {
    public:
        const char * GetClassName() const final { return "LinearizeDepthPass"; }

        LinearizeDepthPass();
        ~LinearizeDepthPass();

        void Setup(const gfx::RenderPassContext & _renderContext) override;
        void Render(const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) const override;

    private:
        gfx::RootSignatureHandle        m_linearizeDepthRootSignature;
        gfx::ShaderKey                  m_linearizeDepthShaderKey;
        gfx::ShaderKey                  m_linearizeDepthMSAA2XShaderKey;
        gfx::ShaderKey                  m_linearizeDepthMSAA4XShaderKey;
        gfx::ShaderKey                  m_linearizeDepthMSAA8XShaderKey;
        gfx::ShaderKey                  m_linearizeDepthMSAA16XShaderKey;
    };
}