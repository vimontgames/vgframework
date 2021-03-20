#include "TestPass2D.h"
#include "shaders/driver/driver.hlsli"
#include "Shaders/system/bindless.hlsli"

#include "core/IInput.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    // Setup executed once, when pass is created
    //--------------------------------------------------------------------------------------
    TestPass2D::TestPass2D()
    {
        auto * device = Device::get();
        
        RootSignatureDesc rsDesc;
        rsDesc.addRootConstants(ShaderStageFlags::VS | ShaderStageFlags::PS, 0, RootConstants2DCount);
        
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
            m_texture.push_back(device->createTexture("projects/supervimontbrawl/data/Textures/Sprites.psd"));
        }
    }

    //--------------------------------------------------------------------------------------
    TestPass2D::~TestPass2D()
    {
        auto * device = Device::get();
        device->removeRootSignature(m_rootSignatureHandle);

        for (auto *& tex : m_texture)
            VG_SAFE_RELEASE_ASYNC(tex);
        m_texture.clear();
    }

    //--------------------------------------------------------------------------------------
    // Setup executed each frame, for each pass instance
    //--------------------------------------------------------------------------------------
    void TestPass2D::setup(double _dt)
    {
        auto * renderer = Renderer::get();
        const auto & backbuffer = renderer->getBackbuffer()->getTexDesc();

        const auto * input = Kernel::getInput();

        const uint joyCount = input->getJoyCount();
        const uint spriteCount = countof(m_testSpriteData);
        const uint count = min(spriteCount, joyCount);
        for (uint i = 0; i < count; ++i)
        {
            auto & data = m_testSpriteData[i];

            const float2 joyDir = input->getJoyDir(i);

            if (joyDir.x > 0)
                data.reverse = false;
            else if (joyDir.x < 0)
                data.reverse = true;

            float speed;

            if (input->isJoyButtonPressed(i, JoyButton::A))
                speed = 0.4f;
            else if (input->isJoyButtonPressed(i, JoyButton::B))
                speed = 0.1f;
            else
                speed = 0.25f;

            data.pos.x += (float)_dt * speed * joyDir.x / (float)backbuffer.width;
            data.pos.y += (float)_dt * speed * joyDir.y / (float)backbuffer.height;

            if (data.pos.x < 0.0f)
                data.pos.x = 0.0f;
            else if (data.pos.x > 0.9)
                data.pos.x = 0.9f;

            if (data.pos.y < 0.0f)
                data.pos.y = 0.0f;
            else if (data.pos.y > 0.9)
                data.pos.y = 0.9f;
        }

        writeRenderTarget(0, "Color");
    }

    //--------------------------------------------------------------------------------------
    void TestPass2D::draw(CommandList * _cmdList) const
    {
        auto * renderer = Renderer::get();
        const auto & backbuffer = renderer->getBackbuffer()->getTexDesc();

        RasterizerState rs(FillMode::Solid, CullMode::None);
        BlendState bs(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha, BlendOp::Add);
        DepthStencilState ds(false);
        
        _cmdList->setRootSignature(m_rootSignatureHandle);
        _cmdList->setShader(m_shaderKey);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleStrip);
        _cmdList->setRasterizerState(rs);
        _cmdList->setBlendState(bs);
        _cmdList->setDepthStencilState(ds);

        auto * device = Device::get();
        auto * bindless = device->getBindlessTable();
          
        const uint atlas_width = 16;
        const uint atlas_height = 16;

        float ar = float(backbuffer.width) / float(backbuffer.height);

        // draw sprites
        for (uint i = 0; i < countof(m_testSpriteData); ++i)
        {
            const auto & data = m_testSpriteData[i];

            RootConstants2D root2D;

            const float spriteWidth = 1.0f / float(atlas_width);
            const float spriteHeight = 1.0f / float(atlas_height);

            root2D.quad.posOffsetScale = float4(data.pos.x, data.pos.y, 0.2f, 0.2f * ar);

            if (data.reverse)
                root2D.quad.uvOffsetScale = float4(spriteWidth, spriteHeight*i, -spriteWidth, spriteHeight);
            else
                root2D.quad.uvOffsetScale = float4(0.0f, spriteHeight*i, spriteWidth, spriteHeight);

            root2D.texID = m_texture[3]->getBindlessSRVHandle();

            _cmdList->setInlineRootConstants(&root2D, RootConstants2DCount);
            _cmdList->draw(4);
        }
    }
}