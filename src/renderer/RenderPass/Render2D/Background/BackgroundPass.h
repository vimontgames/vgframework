#pragma once

#include "Renderer/RenderPass/Render2D/Render2DPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class BackgroundPass : public Render2DPass
    {
    public:
        const char * getClassName() const final { return "BackgroundPass"; }

        BackgroundPass();
        ~BackgroundPass();

        void Setup(const gfx::RenderPassContext & _renderContext, float _dt) override;
        void Render(const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) const override;

    private:
        gfx::RootSignatureHandle    m_backgroundRootSignatureHandle;
        gfx::ShaderKey              m_backgroundShaderKey;
        bool                        m_useFastClear = true;
    };
}