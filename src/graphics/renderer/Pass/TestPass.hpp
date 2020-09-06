namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    TestPass::TestPass()
    {
        auto * device = Device::get();

        RootSignatureDesc rsDesc;

        rsDesc.addRootConstants(ShaderStageFlags::VS | ShaderStageFlags::PS, 0, 4);

        m_rootSignatureHandle = device->addRootSignature(rsDesc);

        ShaderCompiler * shaderCompiler = ShaderCompiler::get();
        m_vs = shaderCompiler->compile("driver/driver.hlsl", "VS_Quad", ShaderStage::Vertex);
        m_ps = shaderCompiler->compile("driver/driver.hlsl", "PS_Quad", ShaderStage::Pixel);
    }

    //--------------------------------------------------------------------------------------
    TestPass::~TestPass()
    {
        auto * device = Device::get();
        device->removeRootSignature(m_rootSignatureHandle);
        VG_SAFE_RELEASE(m_vs);
        VG_SAFE_RELEASE(m_ps);
    }

    //--------------------------------------------------------------------------------------
    void TestPass::setup()
    {
        auto * renderer = Renderer::get();
        auto & backbuffer = renderer->getBackbuffer()->getTexDesc();

        FrameGraph::TextureDesc desc;
        desc.width = backbuffer.width;
        desc.height = backbuffer.height;
        desc.format = PixelFormat::R8G8B8A8_unorm;
        desc.initState = FrameGraph::Resource::InitState::Clear;
        desc.clearColor = { 1,0,0,1 };

        writeRenderTarget(0, "Backbuffer", desc);
    }

    //--------------------------------------------------------------------------------------
    void TestPass::draw(CommandList * _cmdList) const
    {

    }
}