#include "TestPass3D.h"
#include "core/Sector/Sector.h"
#include "shaders/default/default.hlsli"
#include "Shaders/system/bindless.hlsli"
#include "graphics/renderer/Geometry/Mesh/MeshGeometry.h"
#include "graphics/renderer/Model/Mesh/MeshModel.h"
#include "graphics/renderer/View/View.h"
#include "graphics/renderer/Importer/FBX/FBXImporter.h"
#include "graphics/renderer/IGraphicInstance.h"
#include "graphics/renderer/Model/Material/MaterialModel.h"

#include "Shaders/system/vertex.hlsl.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    // Setup executed once, when pass is created
    //--------------------------------------------------------------------------------------
    TestPass3D::TestPass3D()
    {
        auto * device = Device::get();
        auto * renderer = Renderer::get();

        RootSignatureDesc rsDesc;
        rsDesc.addRootConstants(ShaderStageFlags::All, 0, RootConstants3DCount);

        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();
        rsDesc.addTable(bindlessTable);

        m_rootSignatureHandle = device->addRootSignature(rsDesc);

        m_forwardShaderKey.init("default/default.hlsl", "Forward");
        m_wireframeShaderKey.init("default/default.hlsl", "Wireframe");

        const u16 ibData[] =
        {
            0, 1, 2,
            2, 1, 3,
            
            4, 6, 5,
            5, 6, 7,
            
            8, 10, 9,
            9, 10, 11,

            12, 13, 14,
            14, 13, 15,

            16, 17, 18,
            18, 17, 19,

            20, 22, 21,
            21, 22, 23
        };

        const uint indexCount = (uint)countof(ibData);
        BufferDesc ibDesc(Usage::Default, BindFlags::IndexBuffer, CPUAccessFlags::None, BufferFlags::None, sizeof(ibData[0]), indexCount);
        Buffer * ib = device->createBuffer(ibDesc, "CubeIB", ibData);

        const SimpleVertex vbData[] =
        {
            { {-1.0f,-1.0f,-1.0f}, {0.0f, 0.0f, -1.0f}, { 0.0f, 0.0f } },
            { {-1.0f,+1.0f,-1.0f}, {0.0f, 0.0f, -1.0f}, { 0.0f, 1.0f } },
            { {+1.0f,-1.0f,-1.0f}, {0.0f, 0.0f, -1.0f}, { 1.0f, 0.0f } },
            { {+1.0f,+1.0f,-1.0f}, {0.0f, 0.0f, -1.0f}, { 1.0f, 1.0f } },
                                    
            { {-1.0f,-1.0f,+1.0f}, {0.0f, 0.0f, +1.0f}, { 0.0f, 0.0f } },
            { {-1.0f,+1.0f,+1.0f}, {0.0f, 0.0f, +1.0f}, { 0.0f, 1.0f } },
            { {+1.0f,-1.0f,+1.0f}, {0.0f, 0.0f, +1.0f}, { 1.0f, 0.0f } },
            { {+1.0f,+1.0f,+1.0f}, {0.0f, 0.0f, +1.0f}, { 1.0f, 1.0f } },

            { {-1.0f,-1.0f,-1.0f}, {0.0f, -1.0f, 0.0f}, { 1.0f, 0.0f } },
            { {-1.0f,-1.0f,+1.0f}, {0.0f, -1.0f, 0.0f}, { 1.0f, 1.0f } },
            { {+1.0f,-1.0f,-1.0f}, {0.0f, -1.0f, 0.0f}, { 0.0f, 0.0f } },
            { {+1.0f,-1.0f,+1.0f}, {0.0f, -1.0f, 0.0f}, { 0.0f, 1.0f } },

            { {-1.0f,+1.0f,-1.0f}, {0.0f, +1.0f, 0.0f}, { 0.0f, 0.0f } },
            { {-1.0f,+1.0f,+1.0f}, {0.0f, +1.0f, 0.0f}, { 0.0f, 1.0f } },
            { {+1.0f,+1.0f,-1.0f}, {0.0f, +1.0f, 0.0f}, { 1.0f, 0.0f } },
            { {+1.0f,+1.0f,+1.0f}, {0.0f, +1.0f, 0.0f}, { 1.0f, 1.0f } },

            { {-1.0f,-1.0f,-1.0f}, {-1.0f, 0.0f, 0.0f}, { 0.0f, 0.0f } },
            { {-1.0f,-1.0f,+1.0f}, {-1.0f, 0.0f, 0.0f}, { 0.0f, 1.0f } },
            { {-1.0f,+1.0f,-1.0f}, {-1.0f, 0.0f, 0.0f}, { 1.0f, 0.0f } },
            { {-1.0f,+1.0f,+1.0f}, {-1.0f, 0.0f, 0.0f}, { 1.0f, 1.0f } },

            { {+1.0f,-1.0f,-1.0f}, {+1.0f, 0.0f, 0.0f}, { 1.0f, 0.0f } },
            { {+1.0f,-1.0f,+1.0f}, {+1.0f, 0.0f, 0.0f}, { 1.0f, 1.0f } },
            { {+1.0f,+1.0f,-1.0f}, {+1.0f, 0.0f, 0.0f}, { 0.0f, 0.0f } },
            { {+1.0f,+1.0f,+1.0f}, {+1.0f, 0.0f, 0.0f}, { 0.0f, 1.0f } }
        };

        const uint vertexCount = (uint)countof(vbData);
        BufferDesc vbDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(SimpleVertex), vertexCount);
        Buffer * vb = device->createBuffer(vbDesc, "CubeVB", vbData);

        m_meshModel = new MeshModel("CubeModel", this);

        MeshGeometry * meshGeometry = new MeshGeometry("CubeGeometry", this);
        meshGeometry->setIndexBuffer(ib);
        meshGeometry->setVertexBuffer(vb);
        meshGeometry->addBatch(indexCount, 0);

        VG_SAFE_RELEASE(ib);
        VG_SAFE_RELEASE(vb);

        m_meshModel->setGeometry(meshGeometry);

        VG_SAFE_RELEASE(meshGeometry);

        const u32 texData[] =
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
        VG_SAFE_RELEASE(m_meshModel);
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
        writeDepthStencil("DepthStencil");
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

        float4x4 proj = setPerspectiveProjectionRH(fovY, ar, 0.01f, 1000.0f);

        View * view = renderer->getView();
        float4x4 viewProj = mul(view->GetViewInvMatrix(), proj);

        const auto * camSector = view->getCameraSector();
        if (nullptr == camSector)
            return;

        const auto & graphicInstances = camSector->getGraphicInstances();
        
        BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);
        DepthStencilState ds(true, true, ComparisonFunc::LessEqual);

        _cmdList->setRootSignature(m_rootSignatureHandle);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleList);
        _cmdList->setBlendState(bs);
        _cmdList->setDepthStencilState(ds);

        const auto options = DisplayOptions::get();

        u16 flags = 0x0;
        
        if (options->isAlbedoMapsEnabled())
            flags |= FLAG_ALBEDOMAPS;
        
        if (options->isNormalMapsEnabled())
            flags |= FLAG_NORMALMAPS;        

        u16 mode = 0x0;

        switch (options->getDisplayMode())
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(options->getDisplayMode());
                break;

            case DisplayMode::Default:
                mode = MODE_DEFAULT;
                break;

            case DisplayMode::MatID:
                mode = MODE_MATID;
                break;

            case DisplayMode::VSNormal:
                mode = MODE_VS_NORMAL;
                break;

            case DisplayMode::VSTangent:
                mode = MODE_VS_TANGENT;
                break;

            case DisplayMode::VSBinormal:
                mode = MODE_VS_BINORMAL;
                break;

            case DisplayMode::UV0:
                mode = MODE_UV0;
                break;

            case DisplayMode::UV1:
                mode = MODE_UV1;
                break;

            case DisplayMode::Albedo:
                mode = MODE_ALBEDO_MAP;
                break;

            case DisplayMode::PSNormal:
                mode = MODE_NORMAL_MAP;
                break;
        }
		
        auto draw = [=]()
        {
            for (uint i = 0; i < graphicInstances.size(); ++i)
            {
                const IGraphicInstance * instance = graphicInstances[i];

                const MeshModel * model = (MeshModel *)instance->getModel(Lod::Lod0);
                if (nullptr == model)
                    model = m_meshModel;

                const MeshGeometry * geo = model->getGeometry();

                Buffer * vb = geo->getVertexBuffer();
                Buffer * ib = geo->getIndexBuffer();
                const auto & batches = geo->batches();

                RootConstants3D root3D;

                float4x4 world = instance->GetWorldMatrix();
                float4x4 wvp = mul(world, viewProj);

                root3D.mat = transpose(wvp);
                root3D.setBuffer(vb->getBindlessSRVHandle());
                root3D.setFlags(flags);
                root3D.setMode(mode);
                
                _cmdList->setIndexBuffer(ib);

                for (uint i = 0; i < batches.size(); ++i)
                {
                    const auto & batch = batches[i];

                    const auto * material = model->getMaterial(i);

                    driver::Texture * albedoMap = nullptr;
                    driver::Texture * normalMap = nullptr;

                    if (nullptr != material)
                    {
                        albedoMap = material->getTexture(MaterialTextureType::Albedo);
                        normalMap = material->getTexture(MaterialTextureType::Normal);
                    }
                    else
                    {
                        albedoMap = renderer->getDefaultTexture(MaterialTextureType::Albedo);
                        normalMap = renderer->getDefaultTexture(MaterialTextureType::Normal);
                    }

                    root3D.setAlbedoMap(albedoMap->getBindlessSRVHandle());
                    root3D.setNormalMap(normalMap->getBindlessSRVHandle());
                    root3D.setMatID(i);
                    
                    _cmdList->setInlineRootConstants(&root3D, RootConstants3DCount);
                    _cmdList->drawIndexed(batch.count, batch.offset);
                }
            }
        };        

        if (options->isOpaqueEnabled())
        {
            RasterizerState rs(FillMode::Solid, CullMode::Back);

            _cmdList->setShader(m_forwardShaderKey);
            _cmdList->setRasterizerState(rs);

            draw();
        }

        if (options->isWireframeEnabled())
        {
            RasterizerState rs(FillMode::Wireframe, CullMode::None);
            _cmdList->setShader(m_wireframeShaderKey);
            _cmdList->setRasterizerState(rs);
        
            draw();
        }
    }
}