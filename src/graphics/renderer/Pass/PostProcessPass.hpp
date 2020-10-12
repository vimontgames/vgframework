#include "TestPass.h"
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
        rsDesc.addRootConstants(ShaderStageFlags::VS | ShaderStageFlags::PS, 0, sizeof(RootConstants) / sizeof(u32));

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

        writeRenderTarget(0, "Backbuffer");
    }

    //--------------------------------------------------------------------------------------
    void PostProcessPass::draw(CommandList * _cmdList) const
    {
        VG_PROFILE_GPU("PostProcess");

        //RasterizerState rs(FillMode::Solid, CullMode::None);
        //
        //_cmdList->setRootSignature(m_rootSignatureHandle);
        //_cmdList->setShader(m_shaderKey);
        //_cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleStrip);
        //_cmdList->setRasterizerState(rs);
        //
        //float4 posOffetScale, texOffetScale;
        //uint texID;
        //
        //posOffetScale = float4(0.1f, 0.1f, 0.8f, 0.8f);
        //texOffetScale = float4(0.0f, 0.0f, 1.0f, 1.0f);
        //texID = m_texture[0]->getBindlessSRVHandle();
        //
        //_cmdList->setRootConstants(0, (u32*)&posOffetScale, 4);
        //_cmdList->setRootConstants(4, (u32*)&texOffetScale, 4);
        //_cmdList->setRootConstants(8, &texID, 1);
        //
        //_cmdList->draw(4);
        //
        //static float y = 0.0f;
        //
        //posOffetScale = float4(0.75f, 0.25f + y, 0.25f, 0.25f);
        //texOffetScale = float4(0.0f, 0.0f, 1.0f, 1.0f);
        //texID = m_texture[3]->getBindlessSRVHandle();
        //
        //_cmdList->setRootConstants(0, (u32*)&posOffetScale, 4);
        //_cmdList->setRootConstants(4, (u32*)&texOffetScale, 4);
        //_cmdList->setRootConstants(8, &texID, 1);
        //
        //auto * renderer = Renderer::get();
        //const auto & backbuffer = renderer->getBackbuffer()->getTexDesc();
        //
        //static bool reverse = false;
        //
        //if (y < -0.25)
        //    reverse = false;
        //else if (y > 0.5)
        //    reverse = true;
        //
        //if (reverse)
        //    y -= 1.0f / (float)backbuffer.height;
        //else
        //    y += 1.0f / (float)backbuffer.height;
        //
        //_cmdList->draw(4);
    }
}