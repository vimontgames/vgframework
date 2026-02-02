#pragma once
#include "Renderer/RenderPass/Compute/ComputePass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class ComputeScreenSpaceAmbientPass : public ComputePass
    {
    public:
        const char * GetClassName() const final { return "ComputeScreenSpaceAmbientPass"; }

        ComputeScreenSpaceAmbientPass();
        ~ComputeScreenSpaceAmbientPass();

        void Setup(const gfx::RenderPassContext & _renderPassContext) override;
        void Render(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) const override;

    private:
        ScreenSpaceAmbient getScreenSpaceAmbient(const IView * _view) const;

    private:
        gfx::RootSignatureHandle    m_computeScreenSpaceAmbientRootSignature;
        gfx::ComputeShaderKey       m_computeScreenSpaceAmbientShaderKey;
    };
}