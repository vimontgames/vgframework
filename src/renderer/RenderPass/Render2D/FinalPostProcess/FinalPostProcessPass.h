#pragma once

#include "Renderer/RenderPass/Render2D/Render2DPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class FinalPostProcessPass : public Render2DPass
    {
    public:
        const char * getClassName() const final { return "FinalPostProcessPass"; }

        FinalPostProcessPass();
        ~FinalPostProcessPass();

        void setup(const gfx::RenderPassContext & _renderContext, double _dt) override;
        void draw(const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) const override;

    private:
        gfx::RootSignatureHandle         m_postProcessRootSignature;
        gfx::ShaderKey                   m_postProcessShaderKey;
    };
}