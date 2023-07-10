#include "TestPass3D.h"
#include "core/GameObject/GameObject.h"
#include "core/Scene/Scene.h"
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

        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();

        RootSignatureDesc rsDesc;
                          rsDesc.addRootConstants(ShaderStageFlags::All, 0, RootConstants3DCount);
                          rsDesc.addTable(bindlessTable);

        m_rootSignatureHandle = device->addRootSignature(rsDesc);

        m_forwardShaderKey.init("default/default.hlsl", "Forward");
        m_wireframeShaderKey.init("default/default.hlsl", "Wireframe");

        createGrid();
        createAxis();
    }

    //--------------------------------------------------------------------------------------
    TestPass3D::~TestPass3D()
    {
        destroyAxis();
        destroyGrid();

        auto * device = Device::get();
        device->removeRootSignature(m_rootSignatureHandle);
    }

    //--------------------------------------------------------------------------------------
    void TestPass3D::createGrid()
    {
        auto * device = Device::get();

        vector<SimpleVertex> vertices;

        const uint gridSize = 16;

        int begin = -(int)gridSize / 2;
        int end = (int)gridSize / 2;

        for (int i = begin; i <= end; ++i)
        {
            auto & v0 = vertices.emplace_back();
                   v0.setPos({ (float)i, (float)begin, 0.0f });
                   v0.setColor(0xFF0D0D0D);

            auto & v1 = vertices.emplace_back();
                   v1.setPos({ (float)i, (float)end, 0.0f } );
                   v1.setColor(0xFF0D0D0D);

            auto & h0 = vertices.emplace_back();
                   h0.setPos({ (float)begin,(float)i, 0.0f } );
                   h0.setColor(0xFF0D0D0D);

            auto & h1 = vertices.emplace_back();
                   h1.setPos({ (float)end,(float)i, 0.0f } );
                   h1.setColor(0xFF0D0D0D);
        }

        BufferDesc vbDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(SimpleVertex), (u32)vertices.size());

        m_gridVB = device->createBuffer(vbDesc, "GridVB", vertices.data());
    }

    //--------------------------------------------------------------------------------------
    void TestPass3D::destroyGrid()
    {
        VG_SAFE_RELEASE(m_gridVB);
    }

    //--------------------------------------------------------------------------------------
    void TestPass3D::createAxis()
    {
        auto * device = Device::get();

        SimpleVertex vertices[6];

        const float eps = 0.0001f;

        vertices[0].setPos( { 0.0f, 0.0f, eps + 0.0f } );
        vertices[0].setColor(0xFF0000FF);
        vertices[1].setPos( { 1.0f, 0.0f, eps + 0.0f } );
        vertices[1].setColor(0xFF0000FF);
                                 
        vertices[2].setPos( { 0.0f, 0.0f, eps + 0.0f } );
        vertices[2].setColor(0xFF00FF00);
        vertices[3].setPos( { 0.0f, 1.0f, eps + 0.0f } );
        vertices[3].setColor(0xFF00FF00);
                                
        vertices[4].setPos( { 0.0f, 0.0f, eps + 0.0f } );
        vertices[4].setColor(0xFFFF0000);
        vertices[5].setPos( { 0.0f, 0.0f, eps + 1.0f } );
        vertices[5].setColor(0xFFFF0000);

        BufferDesc vbDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(SimpleVertex), (uint)countof(vertices));

        m_axisVB = device->createBuffer(vbDesc, "AxisVB", vertices);
    }

    //--------------------------------------------------------------------------------------
    void TestPass3D::destroyAxis()
    {
        VG_SAFE_RELEASE(m_axisVB);
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
    void addInstanceRecur(vector<const IGraphicInstance*> & _graphicInstanceList, const GameObject * _root)
    {
        if (_root)
        {
            auto& instances = _root->GetGraphicInstances();
            _graphicInstanceList.insert(_graphicInstanceList.begin(), instances.begin(), instances.end());
            auto children = _root->getChildren();
            for (uint i = 0; i < children.count(); ++i)
                addInstanceRecur(_graphicInstanceList, children[i]);
        }
    }

    //--------------------------------------------------------------------------------------
    void TestPass3D::draw(CommandList * _cmdList) const
    {
        auto * renderer = Renderer::get();

        auto * device = Device::get();
        const auto & backbuffer = renderer->getBackbuffer()->getTexDesc();

        View * view = renderer->getView();

        const float fovY = view->getCameraFovY();
        const float2 nearFar = view->getCameraNearFar();
        const float ar = float(backbuffer.width) / float(backbuffer.height);

        float4x4 proj = setPerspectiveProjectionRH(fovY, ar, nearFar.x, nearFar.y);
       
        float4x4 viewProj = mul(view->GetViewInvMatrix(), proj);

        // #TODO #FIXME Shall pass the root sector instead? 
        //const auto * camSector = view->getCameraSector();
        //if (nullptr == camSector)
        //    return;

        // #TODO #TEMPHACK No culling for now just add all instances to the list to draw
        vector<const IGraphicInstance*> graphicInstances;
        //addInstanceRecur(graphicInstances, camSector);

        auto universe = view->GetUniverse();
        const auto count = universe->getSceneCount();
        for (uint i = 0; i < count; ++i)
        {
            auto scene = universe->getScene(i);
            addInstanceRecur(graphicInstances, (GameObject*)scene->GetRoot());
        }

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

            case DisplayMode::VSColor:
                mode = MODE_VS_COLOR;
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
		
        auto draw = [=](bool _wireframe = false)
        {
            for (uint i = 0; i < graphicInstances.size(); ++i)
            {
                const IGraphicInstance * instance = graphicInstances[i];

                const MeshModel * model = (MeshModel *)instance->getModel(Lod::Lod0);
                if (nullptr == model)
                    continue;

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

            draw(true);
        }

        drawGrid(_cmdList, viewProj);
        drawAxis(_cmdList, viewProj);
    }

    //--------------------------------------------------------------------------------------
    void TestPass3D::drawGrid(CommandList * _cmdList, const float4x4 & _viewProj) const
    {
        RasterizerState rs(FillMode::Wireframe, CullMode::None);

        const BufferDesc & gridDesc = m_gridVB->getBufDesc();

        RootConstants3D root3D;

        root3D.mat = transpose(_viewProj);
        root3D.setBuffer(m_gridVB->getBindlessSRVHandle());
        root3D.setMode(MODE_VS_COLOR);

        _cmdList->setRasterizerState(rs);
        _cmdList->setShader(m_forwardShaderKey);
        _cmdList->setInlineRootConstants(&root3D, RootConstants3DCount);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::LineList);
        _cmdList->draw(gridDesc.elementCount);
    }

    //--------------------------------------------------------------------------------------
    void TestPass3D::drawAxis(CommandList * _cmdList, const float4x4 & _viewProj) const
    {
        RasterizerState rs(FillMode::Wireframe, CullMode::None);

        const BufferDesc & gridDesc = m_axisVB->getBufDesc();

        RootConstants3D root3D;

        root3D.mat = transpose(_viewProj);
        root3D.setBuffer(m_axisVB->getBindlessSRVHandle());
        root3D.setMode(MODE_VS_COLOR);

        _cmdList->setRasterizerState(rs);
        _cmdList->setShader(m_forwardShaderKey);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::LineList);

        _cmdList->setInlineRootConstants(&root3D, RootConstants3DCount);
        _cmdList->draw(6);
    }
}