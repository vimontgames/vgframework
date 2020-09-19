#include "shaders/driver/driver.hlsli"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    // Setup exectuted once, when pass is created
    //--------------------------------------------------------------------------------------
    TestPass::TestPass()
    {
        auto * device = Device::get();

        RootSignatureDesc rsDesc;
                          rsDesc.addRootConstants(ShaderStageFlags::VS, 0, 8);

        m_rootSignatureHandle = device->addRootSignature(rsDesc);

        m_shaderKey.init("driver/driver.hlsl", "Quad");
    }

    //--------------------------------------------------------------------------------------
    TestPass::~TestPass()
    {
        auto * device = Device::get();
        device->removeRootSignature(m_rootSignatureHandle);
    }

    //--------------------------------------------------------------------------------------
    // Setup executed each frame, for each pass instance
    //--------------------------------------------------------------------------------------
    void TestPass::setup()
    {
        auto * renderer = Renderer::get();
        auto & backbuffer = renderer->getBackbuffer()->getTexDesc();

        FrameGraph::TextureDesc desc;
        desc.width = backbuffer.width;
        desc.height = backbuffer.height;
        desc.format = backbuffer.format;
        desc.initState = FrameGraph::Resource::InitState::Clear;
        desc.clearColor = { 0, 0, 0, 0 };

        writeRenderTarget(0, "Backbuffer", desc);
    }

    //--------------------------------------------------------------------------------------
    void TestPass::draw(CommandList * _cmdList) const
    {
        RasterizerState rs(FillMode::Solid, CullMode::Back);

        _cmdList->setRootSignature(m_rootSignatureHandle);
        _cmdList->setShader(m_shaderKey);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleStrip);
        _cmdList->setRasterizerState(rs);

        float4 posOffetScale, texOffetScale;

        posOffetScale = float4(0.0f, 0.0f, 1.0f, 1.0f);
        texOffetScale = float4(0.0f, 0.0f, 1.0f, 1.0f);
        _cmdList->setRootConstants(0, (u32*)&posOffetScale, 4);
        _cmdList->setRootConstants(4, (u32*)&texOffetScale, 4);

        _cmdList->draw(4);

        posOffetScale = float4(0.25f, 0.25f, 0.5f, 0.5f);
        texOffetScale = float4(0.0f, 0.0f, 4.0f, 4.0f);
        _cmdList->setRootConstants(0, (u32*)&posOffetScale, 4);
        _cmdList->setRootConstants(4, (u32*)&texOffetScale, 4);

        _cmdList->draw(4);
    }
}