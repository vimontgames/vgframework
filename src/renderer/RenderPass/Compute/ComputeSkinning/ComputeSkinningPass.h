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

        void Setup(const gfx::RenderPassContext & _renderContext, double _dt) override;
        void Render(const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) const override;

        void clearSkins();
        void addSkins(const core::vector<MeshInstance *> * _skinnedMeshes);

    private:
        gfx::RootSignatureHandle                                m_computeSkinningRootSignature;
        gfx::ComputeShaderKey                                   m_computeSkinningShaderKey;
        core::vector<const core::vector<MeshInstance *> *>      m_skinnedMeshes;
    };
}