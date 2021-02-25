#include "TestPass.h"
#include "shaders/driver/driver.hlsli"
#include "Shaders/system/bindless.hlsli"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    // Setup executed once, when pass is created
    //--------------------------------------------------------------------------------------
    TestPass::TestPass()
    {
        auto * device = Device::get();
        
        RootSignatureDesc rsDesc;
        rsDesc.addRootConstants(ShaderStageFlags::VS | ShaderStageFlags::PS, 0, RootConstantsCount/*sizeof(RootConstants)/sizeof(u32)*/);
        
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
            m_texture.push_back(device->createTexture("game/supervimontbrawl/data/Textures/Sprites.psd"));
        }

        // buffer 0
        {
            BufferDesc bufDesc = BufferDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, 16, 1);
        
            const float4 initData = float4(1.0f, 1.0f, 0.0f, 1.0f);
        
            m_buffer.push_back(device->createBuffer(bufDesc, "buf #0", (void*)&initData));
        }

        // cb
        {
            BufferDesc bufDesc = BufferDesc(Usage::Dynamic, BindFlags::ConstantBuffer, CPUAccessFlags::None, BufferFlags::None, sizeof(UniformBufferTest), 1);

            m_constantBuffer = device->createBuffer(bufDesc, "cb #0");
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
            
        VG_SAFE_RELEASE_ASYNC(m_constantBuffer);
    }

    //--------------------------------------------------------------------------------------
    // Setup executed each frame, for each pass instance
    //--------------------------------------------------------------------------------------
    void TestPass::setup(double _dt)
    {
        auto * renderer = Renderer::get();
        const auto & backbuffer = renderer->getBackbuffer()->getTexDesc();

        if (m_reverse)
            m_offset -= _dt * 0.5f / (float)backbuffer.width;
        else
            m_offset += _dt * 0.5f / (float)backbuffer.width;

        if (m_offset < 0.25f)
        {
            m_offset = 0.25f;
            m_reverse = false;
        }
        else if (m_offset > 0.75)
        {
            m_offset = 0.75f;
            m_reverse = true;
        }

        writeRenderTarget(0, "Color");
    }

    //--------------------------------------------------------------------------------------
    void TestPass::draw(CommandList * _cmdList) const
    {
        auto * renderer = Renderer::get();
        const auto & backbuffer = renderer->getBackbuffer()->getTexDesc();

        RasterizerState rs(FillMode::Solid, CullMode::None);
        
        _cmdList->setRootSignature(m_rootSignatureHandle);
        _cmdList->setShader(m_shaderKey);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleStrip);
        _cmdList->setRasterizerState(rs);

        auto * device = Device::get();
        auto * bindless = device->getBindlessTable();
                
        float4x4 proj = float4x4::identity();
        float4x4 view = float4x4::identity();
        float4x4 viewproj = view * proj;
        float4x4 world = float4x4::identity();

        float4x4 wvp;
        
        {
            RootConstants root;

            root.mat = world * viewproj;
            root.quad.posOffsetScale = float4(0.5f, 0.5f, 0.125f, 0.125f);
            root.quad.uvOffsetScale = float4(0.0f, 0.0f, 1.0f, 1.0f);
            root.texID = m_texture[0]->getBindlessSRVHandle();

            _cmdList->setInlineRootConstants(&root, RootConstantsCount);
            _cmdList->draw(4);
        }

        {
            RootConstants root;

            //world.f32_3[0] += m_offset;

            const uint atlas_width = 16;
            const uint atlas_height = 16;

            float ar = float(backbuffer.width) / float(backbuffer.height);

            root.mat = mul(world, viewproj);
            root.quad.posOffsetScale = float4(m_offset, 0.75f, 0.1f, 0.1f * ar);
            root.quad.uvOffsetScale = float4(0.0f, 0.0f, 1.0f / float(atlas_width), 1.0f / float(atlas_height));
            root.texID = m_texture[3]->getBindlessSRVHandle();

            _cmdList->setInlineRootConstants(&root, RootConstantsCount);
            _cmdList->draw(4);
        }
    }
}