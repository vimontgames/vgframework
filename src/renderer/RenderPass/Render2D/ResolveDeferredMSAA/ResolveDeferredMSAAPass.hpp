#include "ResolveDeferredMSAAPass.h"
#include "renderer/Options/RendererOptions.h"
#include "gfx/IView.h"

using namespace vg::gfx;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    ResolveDeferredMSAAPass::ResolveDeferredMSAAPass() :
        Render2DPass("ResolveDeferredMSAAPass")
    {
        auto * device = Device::get();

        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();

        RootSignatureDesc rsDesc;
        rsDesc.addRootConstants(ShaderStageFlags::VS | ShaderStageFlags::PS, 0, 0, RootConstants2DCount);
        rsDesc.addTable(bindlessTable);

        m_resolveDeferredMSAARootSignature = device->addRootSignature(rsDesc);

        m_resolveDeferredMSAA2XShaderKey.init("driver/driver.hlsl", "MSAA2X");
        m_resolveDeferredMSAA4XShaderKey.init("driver/driver.hlsl", "MSAA4X");
        m_resolveDeferredMSAA8XShaderKey.init("driver/driver.hlsl", "MSAA8X");
        m_resolveDeferredMSAA16XShaderKey.init("driver/driver.hlsl", "MSAA16X");      
    }

    //--------------------------------------------------------------------------------------
    ResolveDeferredMSAAPass::~ResolveDeferredMSAAPass()
    {
        auto * device = Device::get();
        device->removeRootSignature(m_resolveDeferredMSAARootSignature);
    }

    //--------------------------------------------------------------------------------------
    void ResolveDeferredMSAAPass::Setup(const RenderPassContext & _renderPassContext)
    {
        readRWTexture(_renderPassContext.getFrameGraphID("ResolveDeferredMSAA"));
        writeRenderTarget(0, _renderPassContext.getFrameGraphID("Color"));
    }

    //--------------------------------------------------------------------------------------
    void ResolveDeferredMSAAPass::Render(const RenderPassContext & _renderPassContext, CommandList * _cmdList) const
    {
        const auto * options = RendererOptions::get();
        const auto msaa = options->GetMSAA();

        RasterizerState rs(FillMode::Solid, CullMode::None);
        BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);
        DepthStencilState ds(false);

        _cmdList->setGraphicRootSignature(m_resolveDeferredMSAARootSignature);

        switch (msaa)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(msaa);

            case MSAA::MSAA2X:
                _cmdList->setShader(m_resolveDeferredMSAA2XShaderKey);
                break;

            case MSAA::MSAA4X:
                _cmdList->setShader(m_resolveDeferredMSAA4XShaderKey);
                break;

            case MSAA::MSAA8X:
                _cmdList->setShader(m_resolveDeferredMSAA8XShaderKey);
                break;

            case MSAA::MSAA16X:
                _cmdList->setShader(m_resolveDeferredMSAA16XShaderKey);
                break;
        }

        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleStrip);
        _cmdList->setRasterizerState(rs);
        _cmdList->setBlendState(bs);
        _cmdList->setDepthStencilState(ds);
        _cmdList->enablePerSampleShading(true);

        RootConstants2D root2D;

        root2D.quad.posOffsetScale = float4(0,0,1,1);
        root2D.quad.uvOffsetScale = float4(0,0,1,1);
        root2D.texID = getRWTexture(_renderPassContext.getFrameGraphID("ResolveDeferredMSAA"))->getTextureHandle();

        _cmdList->setGraphicRootConstants(0, (u32 *)&root2D, RootConstants2DCount);
        _cmdList->draw(4);
        _cmdList->enablePerSampleShading(false);
    }
}