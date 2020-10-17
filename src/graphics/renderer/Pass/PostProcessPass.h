#pragma once

#include "graphics/driver/FrameGraph/UserPass.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    class PostProcessPass : public driver::UserPass
    {
    public:
        PostProcessPass();
        ~PostProcessPass();

        void setup() override;
        void draw(driver::CommandList * _cmdList) const override;

        //const core::string & getName() const override { static const core::string name = "PostProcess"; return name; }

    private:
        driver::RootSignatureHandle         m_postProcessRootSignature;
        driver::ShaderKey                   m_postProcessShaderKey;
        driver::BindlessTextureSrvHandle    m_srcTexHandle;
    };
}