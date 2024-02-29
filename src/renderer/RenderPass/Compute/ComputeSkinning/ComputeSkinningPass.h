#pragma once

#include "Renderer/RenderPass/Compute/ComputePass.h"

namespace vg::renderer
{
    class IGraphicInstance;
    class MeshInstance;

    //--------------------------------------------------------------------------------------
    class ComputeSkinningPass : public ComputePass
    {
    public:
        const char * getClassName() const final { return "ComputeSkinningPass"; }

        ComputeSkinningPass();
        ~ComputeSkinningPass();

        void Setup(const gfx::RenderPassContext & _renderContext) override;
        void Render(const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) const override;

    private:
        gfx::RootSignatureHandle    m_computeSkinningRootSignature;
        gfx::ComputeShaderKey       m_computeSkinningShaderKey;
        gfx::Buffer *               m_skinningMatricesBuffer;
    };
}