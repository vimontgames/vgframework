#include "TexturePreviewPass.h"
#include "Shaders/preview/preview.hlsl.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    TexturePreviewPass::TexturePreviewPass(const core::string & _passName) :
        Render2DPass(_passName)
    {
        auto * device = Device::get();

        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();

        RootSignatureDesc rsDesc;
        rsDesc.addRootConstants(ShaderStageFlags::VS | ShaderStageFlags::PS, 0, 0, RootConstants2DCount);
        rsDesc.addTable(bindlessTable);

        m_texturePreviewRootSignature = device->addRootSignature(rsDesc);
        m_texturePreviewShaderKey.init("preview/preview.hlsl", "Preview");
    }

    //--------------------------------------------------------------------------------------
    TexturePreviewPass::~TexturePreviewPass()
    {
        auto * device = Device::get();
        device->removeRootSignature(m_texturePreviewRootSignature);
    }

    //--------------------------------------------------------------------------------------
    void TexturePreviewPass::Setup(const gfx::RenderPassContext & _renderPassContext)
    {
        writeRenderTarget(0, fmt::sprintf("TexturePreview #%u", m_index));
    }

    //--------------------------------------------------------------------------------------
    void TexturePreviewPass::setSourceTexture(gfx::Texture * _srcTex, core::uint _index, bool _firstUse)
    {
        m_srcTex = _srcTex;
        VG_SAFE_INCREASE_REFCOUNT(m_srcTex);
        m_index = _index;
        m_firstUse = _firstUse;
    }

    //--------------------------------------------------------------------------------------
    void TexturePreviewPass::BeforeRender(const RenderPassContext & _renderContext, CommandList * _cmdList)
    {
        if (!m_firstUse)
        {
            Texture * dstTex = getRenderTarget(fmt::sprintf("TexturePreview #%u", m_index));
            _cmdList->transitionResource(dstTex, ResourceState::ShaderResource, ResourceState::RenderTarget);
        }
    };

    //--------------------------------------------------------------------------------------
    void TexturePreviewPass::Render(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) const
    {
        RasterizerState rs(FillMode::Solid, CullMode::None);
        BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);
        DepthStencilState ds(false);

        const auto & srcDesc = m_srcTex->getTexDesc();
        auto shaderKey = m_texturePreviewShaderKey;

        switch (srcDesc.type)
        {
            case gfx::TextureType::TextureCube:
                shaderKey.setValue(gfx::PreviewHLSLDesc::Flags::Type, 2); // _TEXCUBE
                break;
        }

        _cmdList->setGraphicRootSignature(m_texturePreviewRootSignature);
        _cmdList->setShader(shaderKey);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleStrip);
        _cmdList->setRasterizerState(rs);
        _cmdList->setBlendState(bs);
        _cmdList->setDepthStencilState(ds);

        RootConstants2D root2D;

        root2D.quad.posOffsetScale = float4(0,0,1,1);
        root2D.quad.uvOffsetScale = float4(0,0,1,1);

        root2D.texID = m_srcTex->getTextureHandle();

        _cmdList->setGraphicRootConstants(0, (u32 *)&root2D, RootConstants2DCount);
        _cmdList->draw(4);        
    }

    //--------------------------------------------------------------------------------------
    void TexturePreviewPass::AfterRender(const RenderPassContext & _renderContext, CommandList * _cmdList)
    {
        Texture * dstTex = getRenderTarget(fmt::sprintf("TexturePreview #%u", m_index));
        _cmdList->transitionResource(dstTex, ResourceState::RenderTarget, ResourceState::ShaderResource);
        VG_SAFE_RELEASE_ASYNC(m_srcTex);
    };
}