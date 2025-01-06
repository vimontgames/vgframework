#pragma once

#include "Renderer/RenderPass/Compute/ComputePass.h"

namespace vg::renderer
{
    class IGraphicInstance;
    class MeshInstance;

    //--------------------------------------------------------------------------------------
    class ComputeSkinningPass final : public ComputePass
    {
    public:
        const char * GetClassName() const final { return "ComputeSkinningPass"; }

        ComputeSkinningPass();
        ~ComputeSkinningPass();

        core::u64   GetCostEstimate (const gfx::RenderPassContext & _renderContext) const;
        void        Setup           (const gfx::RenderPassContext & _renderContext) final override;
        void        BeforeAll         (const gfx::RenderPassContext & _renderContext) final override;
        void        Render          (const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) const final override;

    private:
        gfx::RootSignatureHandle    m_computeSkinningRootSignature;
        gfx::ComputeShaderKey       m_computeSkinningShaderKey;
        gfx::Buffer *               m_skinningMatricesBuffer;
        core::uint                  m_mapSize = 0;
    };
}