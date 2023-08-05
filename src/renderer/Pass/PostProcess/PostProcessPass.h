#pragma once

#include "gfx/FrameGraph/UserPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class PostProcessPass : public gfx::UserPass
    {
    public:
        const char * getClassName() const final { return "PostProcessPass"; }

        PostProcessPass();
        ~PostProcessPass();

        void setup(const gfx::RenderContext & _renderContext, double _dt) override;
        void draw(const gfx::RenderContext & _renderContext, gfx::CommandList * _cmdList) const override;

    private:
        gfx::RootSignatureHandle         m_postProcessRootSignature;
        gfx::ShaderKey                   m_postProcessShaderKey;
        gfx::BindlessTextureSrvHandle    m_srcTexHandle;
    };
}