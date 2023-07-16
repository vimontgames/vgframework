#pragma once

#include "graphics/driver/FrameGraph/UserPass.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    class BackgroundPass : public driver::UserPass
    {
    public:
        const char * getClassName() const final { return "BackgroundPass"; }

        BackgroundPass();
        ~BackgroundPass();

        void setup(const driver::RenderContext & _renderContext, double _dt) override;
        void draw(const driver::RenderContext & _renderContext, driver::CommandList * _cmdList) const override;

    private:
        driver::RootSignatureHandle     m_backgroundRootSignatureHandle;
        driver::ShaderKey               m_backgroundShaderKey;
    };
}