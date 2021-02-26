#include "PostProcessPass.h"
#include "shaders/driver/driver.hlsli"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    // Setup executed once, when pass is created
    //--------------------------------------------------------------------------------------
    PostProcessPass::PostProcessPass()
    {
        auto * device = Device::get();

        RootSignatureDesc rsDesc;
        rsDesc.addRootConstants(ShaderStageFlags::VS | ShaderStageFlags::PS, 0, RootConstants2DCount);

        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();
        rsDesc.addTable(bindlessTable);

        m_postProcessRootSignature = device->addRootSignature(rsDesc);
        m_postProcessShaderKey.init("driver/driver.hlsl", "Copy");
    }

    //--------------------------------------------------------------------------------------
    PostProcessPass::~PostProcessPass()
    {
        auto * device = Device::get();
        device->removeRootSignature(m_postProcessRootSignature);
    }

    //--------------------------------------------------------------------------------------
    // Setup executed each frame, for each pass instance
    //--------------------------------------------------------------------------------------
    void PostProcessPass::setup(double _dt)
    {
        readRenderTarget("Color");
        writeRenderTarget(0, "Backbuffer");
    }

    //--------------------------------------------------------------------------------------
    void PostProcessPass::draw(CommandList * _cmdList) const
    {
        RasterizerState rs(FillMode::Solid, CullMode::None);
        BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);

        _cmdList->setRootSignature(m_postProcessRootSignature);
        _cmdList->setShader(m_postProcessShaderKey);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleStrip);
        _cmdList->setRasterizerState(rs);
        _cmdList->setBlendState(bs);

        RootConstants2D root2D;

        root2D.quad.posOffsetScale = float4(0.0f, 0.0f, 1.0f, 1.0f);
        root2D.quad.uvOffsetScale = float4(0.0f, 0.0f, 1.0f, 1.0f);
        root2D.texID = getRenderTarget("Color")->getBindlessSRVHandle();

        _cmdList->setInlineRootConstants(&root2D, RootConstants2DCount);

        _cmdList->draw(4);
    }
}