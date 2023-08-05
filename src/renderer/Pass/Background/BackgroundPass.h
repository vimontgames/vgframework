#pragma once

#include "gfx/FrameGraph/UserPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class BackgroundPass : public gfx::UserPass
    {
    public:
        const char * getClassName() const final { return "BackgroundPass"; }

        BackgroundPass();
        ~BackgroundPass();

        void setup(const gfx::RenderContext & _renderContext, double _dt) override;
        void draw(const gfx::RenderContext & _renderContext, gfx::CommandList * _cmdList) const override;

    private:
        gfx::RootSignatureHandle     m_backgroundRootSignatureHandle;
        gfx::ShaderKey               m_backgroundShaderKey;
    };
}