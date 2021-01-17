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
        rsDesc.addRootConstants(ShaderStageFlags::VS | ShaderStageFlags::PS, 0, RootConstantsCount/*sizeof(RootConstants)/sizeof(u32)*/);

        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();
        rsDesc.addTable(bindlessTable);

        m_postProcessRootSignature = device->addRootSignature(rsDesc);
        m_postProcessShaderKey.init("driver/driver.hlsl", "Quad");
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
    void PostProcessPass::setup()
    {
        readRenderTarget("Color");
        writeRenderTarget(0, "Backbuffer");
    }

    //--------------------------------------------------------------------------------------
    void PostProcessPass::draw(CommandList * _cmdList) const
    {
        RasterizerState rs(FillMode::Solid, CullMode::None);

        _cmdList->setRootSignature(m_postProcessRootSignature);
        _cmdList->setShader(m_postProcessShaderKey);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleStrip);
        _cmdList->setRasterizerState(rs);

        float4 posOffetScale, texOffetScale;
        uint texID;

        posOffetScale = float4(0.0f, 0.0f, 1.0f, 1.0f);
        texOffetScale = float4(0.0f, 0.0f, 1.0f, 1.0f);
        texID = getRenderTarget("Color")->getBindlessSRVHandle();

        _cmdList->setRootConstants(0, (u32*)&posOffetScale, 4);
        _cmdList->setRootConstants(4, (u32*)&texOffetScale, 4);
        _cmdList->setRootConstants(8, &texID, 1);

        _cmdList->draw(4);
    }
}