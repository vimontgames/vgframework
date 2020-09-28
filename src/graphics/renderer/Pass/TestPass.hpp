#include "shaders/driver/driver.hlsli"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    // Setup executed once, when pass is created
    //--------------------------------------------------------------------------------------
    TestPass::TestPass()
    {
        auto * device = Device::get();
        
        RootSignatureDesc rsDesc;
        rsDesc.addRootConstants(ShaderStageFlags::VS | ShaderStageFlags::PS, 0, 8);
        
        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();
        rsDesc.addTable(bindlessTable);
        
        m_rootSignatureHandle = device->addRootSignature(rsDesc);
        
        m_shaderKey.init("driver/driver.hlsl", "Quad");
        
        TextureDesc texDesc = TextureDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, TextureType::Texture2D, PixelFormat::R8G8B8A8_unorm, TextureFlags::None, 8, 8);
        
        // texture 0
        {
            u32 texInitData[8][8];
            for (u32 j = 0; j < 8; ++j)
                for (u32 i = 0; i < 8; ++i)
                    texInitData[j][i] = i << 5 | j << 13 | 0xFF7F0000;
        
            m_texture[0] = device->createTexture(texDesc, "testTex", (void*)texInitData);
        }
        
        // texture 1
        {
            const u32 texInitData[8][8] =
            {
                { 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000 },
                { 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000 },
                { 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF },
                { 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF },
                { 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000 },
                { 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000 },
                { 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF },
                { 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF },
            };
        
            m_texture[1] = device->createTexture(texDesc, "testTex", (void*)texInitData);
        }
    }

    //--------------------------------------------------------------------------------------
    TestPass::~TestPass()
    {
        auto * device = Device::get();
        device->removeRootSignature(m_rootSignatureHandle);

        for (auto *& tex : m_texture)
            VG_SAFE_RELEASE_ASYNC(tex);
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

        static float color = 0.0f;

        if (color > 1.0f)
            color -= 1.0f;

        desc.clearColor = { 1, 0, color, 1 };

        color += 1.0f / (float)backbuffer.height;

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
        
        posOffetScale = float4(0.1f, 0.1f, 0.8f, 0.8f);
        texOffetScale = float4(0.0f, 0.0f, 1.0f, 1.0f);
        _cmdList->setRootConstants(0, (u32*)&posOffetScale, 4);
        _cmdList->setRootConstants(4, (u32*)&texOffetScale, 4);
        
        _cmdList->draw(4);
        
        static float y = 0.0f;
        
        if (y > 1.0f)
            y = -0.75f;
        
        posOffetScale = float4(0.25f, 0.25f + y, 0.5f, 0.5f);
        texOffetScale = float4(0.0f, 0.0f, 4.0f, 4.0f);
        _cmdList->setRootConstants(0, (u32*)&posOffetScale, 4);
        _cmdList->setRootConstants(4, (u32*)&texOffetScale, 4);
        
        y += 0.01f;
        
        _cmdList->draw(4);
    }
}