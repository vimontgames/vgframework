#pragma once

#include "Renderer/RenderPass/Compute/ComputePass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class ComputePostProcessPass : public ComputePass
    {
    public:
        const char * getClassName() const final { return "ComputePostProcessPass"; }

        ComputePostProcessPass();
        ~ComputePostProcessPass();

        void Setup(const gfx::RenderPassContext & _renderContext, double _dt) override;
        void Render(const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) const override;

    private:
        gfx::RootSignatureHandle    m_computePostProcessRootSignature;
        gfx::ComputeShaderKey       m_computePostProcessShaderKey;
    };
}