#pragma once
#include "Renderer/RenderPass/Compute/ComputePass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class ComputeDeferredLightingPass : public ComputePass
    {
    public:
        const char * GetClassName() const final { return "ComputeDeferredLightingPass"; }

        ComputeDeferredLightingPass();
        ~ComputeDeferredLightingPass();

        void Setup(const gfx::RenderPassContext & _renderContext) override;
        void Render(const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) const override;

    private:
        gfx::RootSignatureHandle    m_computeDeferredLightingRootSignature;
        gfx::ComputeShaderKey       m_computeDeferredLightingShaderKey;
    };
}