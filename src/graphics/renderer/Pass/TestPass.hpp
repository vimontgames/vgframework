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
        
        //texture 0
        {
            TextureDesc texDesc = TextureDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, TextureType::Texture2D, PixelFormat::R8G8B8A8_unorm, TextureFlags::None, 8, 8);

            u32 texInitData[8][8];
            for (u32 j = 0; j < 8; ++j)
                for (u32 i = 0; i < 8; ++i)
                    texInitData[j][i] = i << 5 | j << 13 | 0xFF7F0000;

            m_texture.push_back(device->createTexture(texDesc, "tex2D #0", (void*)texInitData));
        }

        // texture 1
        {
            TextureDesc texDesc = TextureDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, TextureType::Texture2D, PixelFormat::R8G8B8A8_unorm, TextureFlags::None, 8, 8);

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

            m_texture.push_back(device->createTexture(texDesc, "tex2D #1", (void*)texInitData));
        }

        // texture 2
        {
            TextureDesc texDesc = TextureDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, TextureType::Texture1D, PixelFormat::R8G8B8A8_unorm, TextureFlags::None, 4);

            const u32 texInitData[4] =
            {
                0xFF0000FF, 0xFF00FFFF, 0xFFFFFF00, 0xFFFF0000
            };

            m_texture.push_back(device->createTexture(texDesc, "tex1D #2", (void*)texInitData));
        }

        // texture 3
        {
            TextureDesc texDesc = TextureDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, TextureType::Texture2D, PixelFormat::R8G8B8A8_unorm, TextureFlags::None, 2, 2);

            const u32 texInitData[4] =
            {
                0xFF0000FF, 0xFF00FF00,
                0xFFFF0000, 0xFF7F7F7F
            };

            m_texture.push_back(device->createTexture(texDesc, "tex1D #3", (void*)texInitData));
        }

        // buffer 0
        {
            BufferDesc bufDesc = BufferDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, 16, 1);
        
            const float4 initData = float4(0.5f, 0.6f, 0.7f, 0.8f);
        
            m_buffer.push_back(device->createBuffer(bufDesc, "buf #0", (void*)&initData));
        }
    }

    //--------------------------------------------------------------------------------------
    TestPass::~TestPass()
    {
        auto * device = Device::get();
        device->removeRootSignature(m_rootSignatureHandle);

        for (auto *& tex : m_texture)
            VG_SAFE_RELEASE_ASYNC(tex);
        m_texture.clear();

        for (auto *& buf : m_buffer)
            VG_SAFE_RELEASE_ASYNC(buf);
        m_buffer.clear();
    }

    //--------------------------------------------------------------------------------------
    // Setup executed each frame, for each pass instance
    //--------------------------------------------------------------------------------------
    void TestPass::setup()
    {
        auto * renderer = Renderer::get();
        const auto & backbuffer = renderer->getBackbuffer()->getTexDesc();

        FrameGraph::TextureDesc desc;
        desc.width = backbuffer.width;
        desc.height = backbuffer.height;
        desc.format = backbuffer.format;
        desc.initState = FrameGraph::Resource::InitState::Clear;

        static float color = 0.0f;

        if (color > 1.0f)
            color -= 1.0f;

        desc.clearColor = { 1, 0, color, 1 };

        color += 1.0f / 255.0f;

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
                
        posOffetScale = float4(0.25f, 0.25f + y, 0.5f, 0.5f);
        texOffetScale = float4(0.0f, 0.0f, 2.0f, 2.0f);
        _cmdList->setRootConstants(0, (u32*)&posOffetScale, 4);
        _cmdList->setRootConstants(4, (u32*)&texOffetScale, 4);
        
        auto * renderer = Renderer::get();
        const auto & backbuffer = renderer->getBackbuffer()->getTexDesc();

        static bool reverse = false;

        if (y < -0.25)
            reverse = false;
        else if (y > 0.25)
            reverse = true;

        if (reverse)
            y -= 1.0f / (float)backbuffer.height;
        else
            y += 1.0f/(float)backbuffer.height;
        
        _cmdList->draw(4);
    }
}