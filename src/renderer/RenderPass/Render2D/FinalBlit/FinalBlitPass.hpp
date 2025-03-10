#include "FinalBlitPass.h"
#include "shaders/system/rootConstants2D.hlsli"
#include "renderer/UI/UIRenderer.h"
#include "shaders/driver/driver.hlsl.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    // Setup executed once, when pass is created
    //--------------------------------------------------------------------------------------
    FinalBlitPass::FinalBlitPass() :
        Render2DPass("FinalPostProcessPass")
    {
        auto * device = Device::get();

        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();

        RootSignatureDesc rsDesc;
                          rsDesc.addRootConstants(ShaderStageFlags::VS | ShaderStageFlags::PS, 0, 0, RootConstants2DCount);
                          rsDesc.addTable(bindlessTable);

        m_postProcessRootSignature = device->addRootSignature(rsDesc);
        m_postProcessShaderKeyCopy.init("driver/driver.hlsl", "Copy");
        m_postProcessShaderKeyGamma.init("driver/driver.hlsl", "Gamma");
    }

    //--------------------------------------------------------------------------------------
    FinalBlitPass::~FinalBlitPass()
    {
        auto * device = Device::get();
        device->removeRootSignature(m_postProcessRootSignature);
    }

    //--------------------------------------------------------------------------------------
    // Setup executed each frame, for each pass instance
    //--------------------------------------------------------------------------------------
    void FinalBlitPass::Setup(const gfx::RenderPassContext & _renderPassContext)
    {
        const auto * renderer = Renderer::get();
        const auto options = RendererOptions::get();
        const auto * view = (IView *)_renderPassContext.getView();

        if (view->IsComputePostProcessNeeded())
            readRWTexture(_renderPassContext.getFrameGraphID("PostProcessUAV"));
        else
            readRenderTarget(_renderPassContext.getFrameGraphID("Color"));        

        if (!view->GetRenderTarget() || renderer->IsFullscreen())
        {
            if (HDR::None != renderer->GetHDR())
                writeRenderTarget(0, "HDROutput");
            else
                writeRenderTarget(0, "Backbuffer");
        }
        else
        {
            writeRenderTarget(0, _renderPassContext.MakeFrameGraphID("Dest", view->GetViewport()->GetViewportID()));
        }
    }

    //--------------------------------------------------------------------------------------
    void FinalBlitPass::Render(const RenderPassContext & _renderPassContext, CommandList * _cmdList) const
    {
        const auto * renderer = Renderer::get();
        const auto * options = RendererOptions::get();
        const auto * view = (IView *)_renderPassContext.getView();

        RasterizerState rs(FillMode::Solid, CullMode::None);
        BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);
        DepthStencilState ds(false);

        ShaderKey postProcessShaderKey;

        // Always apply gamma curve when copying
        postProcessShaderKey = m_postProcessShaderKeyGamma;

        _cmdList->setGraphicRootSignature(m_postProcessRootSignature);
        _cmdList->setShader(postProcessShaderKey);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleStrip);
        _cmdList->setRasterizerState(rs);
        _cmdList->setBlendState(bs);
        _cmdList->setDepthStencilState(ds);

        RootConstants2D root2D;

        root2D.quad.posOffsetScale = float4(view->GetViewportOffset(), view->GetViewportScale());
        root2D.quad.uvOffsetScale = float4(0.0f, 0.0f, 1.0f, 1.0f);

        // When Compute post-process is enabled then we blit from the PostProcessUAV read as Shader Resource
        if (view->IsComputePostProcessNeeded())
            root2D.texID = getRWTexture(_renderPassContext.getFrameGraphID("PostProcessUAV"))->getTextureHandle();
        else
            root2D.texID = getRenderTarget(_renderPassContext.getFrameGraphID("Color"))->getTextureHandle();       

        _cmdList->setGraphicRootConstants(0, (u32*)&root2D, RootConstants2DCount);
        _cmdList->draw(4);
    }
}