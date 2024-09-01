#include "HDROutputPass.h"
#include "shaders/system/rootConstants2D.hlsli"
#include "shaders/driver/driver.hlsl.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    // Setup executed once, when pass is created
    //--------------------------------------------------------------------------------------
    HDROutputPass::HDROutputPass() :
        Render2DPass("HDROutputPass")
    {
        auto * device = Device::get();
        
        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();
        
        RootSignatureDesc rsDesc;
        rsDesc.addRootConstants(ShaderStageFlags::VS | ShaderStageFlags::PS, 0, 0, RootConstants2DCount);
        rsDesc.addTable(bindlessTable);
        
        m_hdrOutputRootSignature = device->addRootSignature(rsDesc);
        m_hdrOutputCopyShaderKey.init("driver/driver.hlsl", "Copy");
        m_hdrOutputHDR10ShaderKey.init("driver/driver.hlsl", "HDR10");
        m_hdrOutputHDR16ShaderKey.init("driver/driver.hlsl", "HDR16");
    }

    //--------------------------------------------------------------------------------------
    HDROutputPass::~HDROutputPass()
    {
        auto * device = Device::get();
        device->removeRootSignature(m_hdrOutputRootSignature);
    }

    //--------------------------------------------------------------------------------------
    // Setup executed each frame, for each pass instance
    //--------------------------------------------------------------------------------------
    void HDROutputPass::Setup(const gfx::RenderPassContext & _renderPassContext)
    {
        const auto * renderer = Renderer::get();
        const auto options = RendererOptions::get();
        
        readRenderTarget("HDROutput");
        writeRenderTarget(0, "Backbuffer");
    }

    //--------------------------------------------------------------------------------------
    void HDROutputPass::Render(const RenderPassContext & _renderPassContext, CommandList * _cmdList) const
    {
        const auto renderer = Renderer::get();
        const auto options = RendererOptions::get();
        
        RasterizerState rs(FillMode::Solid, CullMode::None);
        BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);
        DepthStencilState ds(false);

        _cmdList->setGraphicRootSignature(m_hdrOutputRootSignature);
        
        const auto hdrMode = renderer->GetHDR();
        switch (hdrMode)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(hdrMode);

            case gfx::HDR::None:
                _cmdList->setShader(m_hdrOutputCopyShaderKey);
                break;

            case gfx::HDR::HDR10:
                _cmdList->setShader(m_hdrOutputHDR10ShaderKey);
                break;

            case gfx::HDR::HDR16:
                _cmdList->setShader(m_hdrOutputHDR16ShaderKey);
                break;
        }
        
        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleStrip);
        _cmdList->setRasterizerState(rs);
        _cmdList->setBlendState(bs);
        _cmdList->setDepthStencilState(ds);
        
        RootConstants2D root2D;
        root2D.quad.reset();
        root2D.texID = getRenderTarget("HDROutput")->getTextureHandle();
        
        _cmdList->setGraphicRootConstants(0, (u32 *)&root2D, RootConstants2DCount);
        _cmdList->draw(4);
    }
}