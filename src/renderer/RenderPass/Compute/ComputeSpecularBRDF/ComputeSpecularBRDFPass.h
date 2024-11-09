#pragma once

#include "Renderer/RenderPass/Compute/ComputePass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class ComputeSpecularBRDFPass : public ComputePass
    {
    public:
        const char * GetClassName() const final { return "ComputeSpecularBRDFPass"; }

        ComputeSpecularBRDFPass();
        ~ComputeSpecularBRDFPass();

        void Setup(const gfx::RenderPassContext & _renderContext) override;
        void Render(const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) const override;

        void setSpecularBRDFTexture(gfx::Texture * _texture);

    private:
        gfx::RootSignatureHandle    m_computeSpecularBRDFRootSignature;
        gfx::ComputeShaderKey       m_computeSpecularBRDFShaderKey;
        gfx::Texture *              m_specularBRDFTexture = nullptr;
    };
}