#include "TestPass3D.h"
#include "shaders/default/default.hlsli"
#include "Shaders/system/bindless.hlsli"

#include "graphics/renderer/View/View.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    // Setup executed once, when pass is created
    //--------------------------------------------------------------------------------------
    TestPass3D::TestPass3D()
    {
        auto * device = Device::get();

        RootSignatureDesc rsDesc;
        rsDesc.addRootConstants(ShaderStageFlags::All, 0, RootConstants3DCount);

        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();
        rsDesc.addTable(bindlessTable);

        m_rootSignatureHandle = device->addRootSignature(rsDesc);

        m_forwardShaderKey.init("default/default.hlsl", "Forward");
        m_wireframeShaderKey.init("default/default.hlsl", "Wireframe");

        u16 ibData[] =
        {
            0, 2, 1,
            1, 2, 3,
            
            4, 5, 6,
            6, 5, 7,
            
            8, 9, 10,
            10, 9, 11,

            12, 14, 13,
            13, 14, 15,

            16, 18, 17,
            17, 18, 19,

            20, 21, 22,
            22, 21, 23
        };
        BufferDesc ibDesc(Usage::Default, BindFlags::IndexBuffer, CPUAccessFlags::None, BufferFlags::None, sizeof(ibData[0]), (uint)countof(ibData));
        m_ib = device->createBuffer(ibDesc, "CubeIB", ibData);

        struct Vertex
        {
            float pos[3];
            float uv[2];
        };

        Vertex vbData[] =
        {
            { {-1.0f,-1.0f,-1.0f}, { 0.0f, 0.0f } },
            { {-1.0f,+1.0f,-1.0f}, { 0.0f, 1.0f } },
            { {+1.0f,-1.0f,-1.0f}, { 1.0f, 0.0f } },
            { {+1.0f,+1.0f,-1.0f}, { 1.0f, 1.0f } },
                                    
            { {-1.0f,-1.0f,+1.0f}, { 0.0f, 0.0f } },
            { {-1.0f,+1.0f,+1.0f}, { 0.0f, 1.0f } },
            { {+1.0f,-1.0f,+1.0f}, { 1.0f, 0.0f } },
            { {+1.0f,+1.0f,+1.0f}, { 1.0f, 1.0f } },

            { {-1.0f,-1.0f,-1.0f}, { 1.0f, 0.0f } },
            { {-1.0f,-1.0f,+1.0f}, { 1.0f, 1.0f } },
            { {+1.0f,-1.0f,-1.0f}, { 0.0f, 0.0f } },
            { {+1.0f,-1.0f,+1.0f}, { 0.0f, 1.0f } },

            { {-1.0f,+1.0f,-1.0f}, { 0.0f, 0.0f } },
            { {-1.0f,+1.0f,+1.0f}, { 0.0f, 1.0f } },
            { {+1.0f,+1.0f,-1.0f}, { 1.0f, 0.0f } },
            { {+1.0f,+1.0f,+1.0f}, { 1.0f, 1.0f } },

            { {-1.0f,-1.0f,-1.0f}, { 0.0f, 0.0f } },
            { {-1.0f,-1.0f,+1.0f}, { 0.0f, 1.0f } },
            { {-1.0f,+1.0f,-1.0f}, { 1.0f, 0.0f } },
            { {-1.0f,+1.0f,+1.0f}, { 1.0f, 1.0f } },

            { {+1.0f,-1.0f,-1.0f}, { 1.0f, 0.0f } },
            { {+1.0f,-1.0f,+1.0f}, { 1.0f, 1.0f } },
            { {+1.0f,+1.0f,-1.0f}, { 0.0f, 0.0f } },
            { {+1.0f,+1.0f,+1.0f}, { 0.0f, 1.0f } }
        };

        BufferDesc vbDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(vbData), 1);
        m_vb = device->createBuffer(vbDesc, "CubeVB", vbData);

        u32 texData[] =
        {
            0xFF0000FF,
            0xFF00FF00,
            0xFFFF0000,
            0xFFFFFFFF
        };

        TextureDesc texDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, TextureType::Texture2D, PixelFormat::R8G8B8A8_unorm, TextureFlags::None, 2, 2, 1, 1);
        m_texture = device->createTexture("data/Textures/QuestionBox.psd");
    }

    //--------------------------------------------------------------------------------------
    TestPass3D::~TestPass3D()
    {
        VG_SAFE_RELEASE(m_ib);
        VG_SAFE_RELEASE(m_vb);
        VG_SAFE_RELEASE(m_texture);

        auto * device = Device::get();
        device->removeRootSignature(m_rootSignatureHandle);
    }

    //--------------------------------------------------------------------------------------
    // Setup executed each frame, for each pass instance
    //--------------------------------------------------------------------------------------
    void TestPass3D::setup(double _dt)
    {
        writeRenderTarget(0, "Color");
    }

    //--------------------------------------------------------------------------------------
    // sx = sy/_ar
    // sy = 1.0f/tan(fov*0.5f)  
    // q = far / (near - far);
    //
    // sx   0    0     0
    //  0  sy    0     0
    //  0   0    q    -1
    //  0   0   zn*q   0
    //--------------------------------------------------------------------------------------
    float4x4 setPerspectiveProjectionRH(float _fov, float _ar, float _near, float _far)
    {
        const float sy = 1.0f / tan(_fov*0.5f);
        const float sx = sy / _ar;
        const float q = _far / (_near - _far);
        const float nq = _near * q;

        float4x4 mProj
        (
            sx, 0, 0,  0,
            0, sy, 0,  0,
            0, 0,  q, -1,
            0, 0, nq,  0
        );

        return mProj;
    }

    //--------------------------------------------------------------------------------------
    void TestPass3D::draw(CommandList * _cmdList) const
    {
        auto * renderer = Renderer::get();

        auto * device = Device::get();
        const auto & backbuffer = renderer->getBackbuffer()->getTexDesc();

        const float fovY = pi / 4.0f;
        const float ar = float(backbuffer.width) / float(backbuffer.height);

        RasterizerState rs(FillMode::Solid, CullMode::Back);
        BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);

        _cmdList->setRootSignature(m_rootSignatureHandle);
        _cmdList->setShader(m_forwardShaderKey);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleList);
        _cmdList->setRasterizerState(rs);
        _cmdList->setBlendState(bs);

        float4x4 proj = setPerspectiveProjectionRH(fovY, ar, 0.001f, 1000.0f);

        View * view = renderer->getView();
       
        float4x4 world = float4x4::identity();

        float4x4 viewProj = mul(view->getViewInvMatrix(), proj);
        float4x4 wvp = mul(world, viewProj);

        RootConstants3D root3D;

        root3D.mat = transpose(viewProj);
        root3D.data.x = m_vb->getBindlessSRVHandle();
        root3D.data.y = m_texture->getBindlessSRVHandle();

        _cmdList->setInlineRootConstants(&root3D, RootConstants3DCount);
        _cmdList->setIndexBuffer(m_ib);
        _cmdList->drawIndexed(m_ib->getBufDesc().elementCount);

        RasterizerState rsWireframe(FillMode::Wireframe, CullMode::None);
        _cmdList->setShader(m_wireframeShaderKey);
        _cmdList->setRasterizerState(rsWireframe);
        _cmdList->drawIndexed(m_ib->getBufDesc().elementCount);
    }
}