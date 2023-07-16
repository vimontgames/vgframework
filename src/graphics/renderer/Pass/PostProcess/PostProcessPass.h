#pragma once

#include "graphics/driver/FrameGraph/UserPass.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    class PostProcessPass : public driver::UserPass
    {
    public:
        const char * getClassName() const final { return "PostProcessPass"; }

        PostProcessPass();
        ~PostProcessPass();

        void setup(const driver::RenderContext & _renderContext, double _dt) override;
        void draw(const driver::RenderContext & _renderContext, driver::CommandList * _cmdList) const override;

    private:
        driver::RootSignatureHandle         m_postProcessRootSignature;
        driver::ShaderKey                   m_postProcessShaderKey;
        driver::BindlessTextureSrvHandle    m_srcTexHandle;
    };
}