#include "FinalPostProcessPass.h"
#include "shaders/system/rootConstants2D.hlsli"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    // Setup executed once, when pass is created
    //--------------------------------------------------------------------------------------
    FinalPostProcessPass::FinalPostProcessPass() :
        Render2DPass("FinalPostProcessPass")
    {
        auto * device = Device::get();

        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();

        RootSignatureDesc rsDesc;
                          rsDesc.addRootConstants(ShaderStageFlags::VS | ShaderStageFlags::PS, 0, 0, RootConstants2DCount);
                          rsDesc.addTable(bindlessTable);

        m_postProcessRootSignature = device->addRootSignature(rsDesc);
        m_postProcessShaderKey.init("driver/driver.hlsl", "Gamma");
    }

    //--------------------------------------------------------------------------------------
    FinalPostProcessPass::~FinalPostProcessPass()
    {
        auto * device = Device::get();
        device->removeRootSignature(m_postProcessRootSignature);
    }

    //--------------------------------------------------------------------------------------
    // Setup executed each frame, for each pass instance
    //--------------------------------------------------------------------------------------
    void FinalPostProcessPass::setup(const gfx::RenderPassContext & _renderPassContext, double _dt)
    {
        readRenderTarget(_renderPassContext.getFrameGraphID("Color"));
        writeRenderTarget(0, _renderPassContext.getFrameGraphID("Dest")); // TODO: render to "Backbuffer" in exclusive GameMode?
    }

    //--------------------------------------------------------------------------------------
    void FinalPostProcessPass::draw(const RenderPassContext & _renderPassContext, CommandList * _cmdList) const
    {
        RasterizerState rs(FillMode::Solid, CullMode::None);
        BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);
        DepthStencilState ds(false);

        _cmdList->setGraphicRootSignature(m_postProcessRootSignature);
        _cmdList->setShader(m_postProcessShaderKey);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleStrip);
        _cmdList->setRasterizerState(rs);
        _cmdList->setBlendState(bs);
        _cmdList->setDepthStencilState(ds);

        RootConstants2D root2D;

        root2D.quad.posOffsetScale = float4(0.0f, 0.0f, 1.0f, 1.0f);
        root2D.quad.uvOffsetScale = float4(0.0f, 0.0f, 1.0f, 1.0f);
        root2D.texID = getRenderTarget(_renderPassContext.getFrameGraphID("Color"))->getBindlessSRVHandle();

        _cmdList->setInlineRootConstants(&root2D, RootConstants2DCount);

        _cmdList->draw(4);
    }
}