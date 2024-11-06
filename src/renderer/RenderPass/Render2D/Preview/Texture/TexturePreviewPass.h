#pragma once

#include "renderer/RenderPass/Render2D/Render2DPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class TexturePreviewPass : public Render2DPass
    {
    public:
        TexturePreviewPass(const core::string & _passName);
        ~TexturePreviewPass();

        void Setup(const gfx::RenderPassContext & _renderPassContext) override;
        void BeforeRender(const  gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) override;
        void Render(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) const override;
        void AfterRender(const  gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) override;

        void setSourceTexture(gfx::Texture * _srcTex, core::uint _index, bool _firstUse);

    private:
        gfx::RootSignatureHandle    m_texturePreviewRootSignature;
        gfx::ShaderKey              m_texturePreviewShaderKey;
        gfx::Texture *              m_srcTex = nullptr;
        core::uint                  m_index = 0;
        bool                        m_firstUse = false;
    };
}