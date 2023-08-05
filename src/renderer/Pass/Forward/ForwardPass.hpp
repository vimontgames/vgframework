#include "ForwardPass.h"

#include "core/GameObject/GameObject.h"
#include "core/Scene/Scene.h"

#include "renderer/Geometry/Mesh/MeshGeometry.h"
#include "renderer/Model/Mesh/MeshModel.h"
#include "renderer/View/View.h"
#include "renderer/Importer/FBX/FBXImporter.h"
#include "renderer/IGraphicInstance.h"
#include "renderer/Model/Material/MaterialModel.h"

#include "Shaders/system/vertex.hlsl.h"
#include "shaders/default/default.hlsli"
#include "Shaders/system/bindless.hlsli"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    // Setup executed once, when pass is created
    //--------------------------------------------------------------------------------------
    ForwardPass::ForwardPass() :
        gfx::UserPass("ForwardPass")
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

        // TODO: create once and share
        createGrid();
        createAxis();
        createUnitBox();
    }

    //--------------------------------------------------------------------------------------
    ForwardPass::~ForwardPass()
    {
        destroyAxis();
        destroyGrid();
        destroyUnitBox();

        auto * device = Device::get();
        device->removeRootSignature(m_rootSignatureHandle);
    }

    //--------------------------------------------------------------------------------------
    void ForwardPass::createGrid()
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
    void ForwardPass::destroyGrid()
    {
        VG_SAFE_RELEASE(m_gridVB);
    }

    //--------------------------------------------------------------------------------------
    void ForwardPass::createAxis()
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
    void ForwardPass::createUnitBox()
    {
        auto * device = Device::get();

        SimpleVertex vertices[8];

        vertices[0].setPos({ -1.0f, -1.0f, -1.0f });
        vertices[0].setColor(0xFFFFFFFF);

        vertices[1].setPos({ +1.0f, -1.0f, -1.0f });
        vertices[1].setColor(0xFFFFFFFF);

        vertices[2].setPos({ +1.0f, +1.0f, -1.0f });
        vertices[2].setColor(0xFFFFFFFF);

        vertices[3].setPos({ -1.0f, +1.0f, -1.0f });
        vertices[3].setColor(0xFFFFFFFF);

        vertices[4].setPos({ -1.0f, -1.0f, +1.0f });
        vertices[4].setColor(0xFFFFFFFF);

        vertices[5].setPos({ +1.0f, -1.0f, +1.0f });
        vertices[5].setColor(0xFFFFFFFF);

        vertices[6].setPos({ +1.0f, +1.0f, +1.0f });
        vertices[6].setColor(0xFFFFFFFF);

        vertices[7].setPos({ -1.0f, +1.0f, +1.0f });
        vertices[7].setColor(0xFFFFFFFF);

        BufferDesc vbDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(SimpleVertex), (uint)countof(vertices));
        m_unitBoxVB = device->createBuffer(vbDesc, "unitBoxVB", vertices);

        u16 indices[24] =
        {
            0,1,
            1,2,
            2,3,
            3,0,

            4,5,
            5,6,
            6,7,
            7,4,

            0,4,
            3,7,
            1,5,
            2,6
        };

        BufferDesc ibDesc(Usage::Default, BindFlags::IndexBuffer, CPUAccessFlags::None, BufferFlags::None, sizeof(u16), (uint)countof(indices));
        m_unitBoxIB = device->createBuffer(ibDesc, "unitBoxIB", indices);
    }

    //--------------------------------------------------------------------------------------
    void ForwardPass::destroyUnitBox()
    {
        VG_SAFE_RELEASE(m_unitBoxIB);
        VG_SAFE_RELEASE(m_unitBoxVB);
    }

    //--------------------------------------------------------------------------------------
    void ForwardPass::destroyAxis()
    {
        VG_SAFE_RELEASE(m_axisVB);
    }

    //--------------------------------------------------------------------------------------
    // Setup executed each frame, for each pass instance
    //--------------------------------------------------------------------------------------
    void ForwardPass::setup(const gfx::RenderContext & _renderContext, double _dt)
    {
        writeRenderTarget(0, _renderContext.getName("Color"));
        writeDepthStencil(_renderContext.getName("DepthStencil"));
    }

    //--------------------------------------------------------------------------------------
    void addInstanceRecur(vector<const IGraphicInstance*> & _graphicInstanceList, const GameObject * _root)
    {
        if (_root)
        {
            if (GameObject::Flags::Enabled & _root->getFlags())
            {
                auto & instances = _root->GetGraphicInstances();
                _graphicInstanceList.insert(_graphicInstanceList.begin(), instances.begin(), instances.end());
                auto children = _root->getChildren();
                for (uint i = 0; i < children.count(); ++i)
                {
                    auto & child = children[i];
                    addInstanceRecur(_graphicInstanceList, child);
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void ForwardPass::draw(const RenderContext & _renderContext, CommandList * _cmdList) const
    {
        auto * renderer = Renderer::get();

        // Get ViewProj from View's Camera
        View * view = (View *)_renderContext.m_view;
        float4x4 viewProj = view->getViewProjMatrix();

        // TODO: No culling for now just add all instances to the list to draw
        //vector<const IGraphicInstance*> graphicInstances;
        //auto universe = view->GetUniverse();
        //const auto count = universe->getSceneCount();
        //for (uint i = 0; i < count; ++i)
        //{
        //    auto scene = universe->getScene(i);
        //    addInstanceRecur(graphicInstances, (GameObject*)scene->GetRoot());
        //}

        const auto & graphicInstances = view->m_cullingJobResult.m_visibleGraphicInstances;

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

                float4x4 world = instance->getWorldMatrix();
                float4x4 wvp = mul(world, viewProj);

                root3D.mat = transpose(wvp);
                root3D.setBuffer(vb->getBindlessSRVHandle());
                root3D.setFlags(flags);
                root3D.setMode(mode);
                root3D.color = instance->getColor();
                
                _cmdList->setIndexBuffer(ib);

                for (uint i = 0; i < batches.size(); ++i)
                {
                    const auto & batch = batches[i];

                    const auto * material = model->getMaterial(i);

                    gfx::Texture * albedoMap = nullptr;
                    gfx::Texture * normalMap = nullptr;

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
            _cmdList->setRasterizerState(rs);
            _cmdList->setShader(m_wireframeShaderKey);

            draw(true);
        }

        if (options->isAABBEnabled())
        {
            for (uint i = 0; i < graphicInstances.size(); ++i)
            {
                const IGraphicInstance * instance = graphicInstances[i];
                const MeshModel * model = (MeshModel *)instance->getModel(Lod::Lod0);
                if (nullptr == model)
                    continue;

                const MeshGeometry * geo = model->getGeometry();
                drawAABB(_cmdList, geo->getAABB(), instance->getWorldMatrix(), viewProj);
            }
        }

        drawGrid(_cmdList, viewProj);
        drawAxis(_cmdList, viewProj);
    }

    //--------------------------------------------------------------------------------------
    void ForwardPass::drawAABB(CommandList * _cmdList, const AABB & _aabb, const float4x4 & _world, const float4x4 & _viewProj) const
    {
        RasterizerState rs(FillMode::Wireframe, CullMode::None);
        _cmdList->setRasterizerState(rs);


        float3 aabbScale = _aabb.m_max - _aabb.m_min;
        float3 center = _aabb.m_max + _aabb.m_min;
        float3 translation = _world._m30_m31_m32 + center * 0.5f;

        float4x4 scale = float4x4(
            float4(aabbScale.xxx*0.5f, 0),
            float4(aabbScale.yyy*0.5f, 0),
            float4(aabbScale.zzz*0.5f, 0),
            float4(0,0,0, 1)
        );
        float4x4 aabbMatrix = scale * _world;
        aabbMatrix._m30_m31_m32 = translation;
        float4x4 wvp = mul(aabbMatrix, _viewProj);

        RootConstants3D root3D;
        {
            root3D.mat = transpose(wvp);
            root3D.setBuffer(m_unitBoxVB->getBindlessSRVHandle());
            root3D.setMode(MODE_VS_COLOR);

        }

        // Root sig
        _cmdList->setRootSignature(m_rootSignatureHandle);

        // Geometry
        _cmdList->setIndexBuffer(m_unitBoxIB);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::LineList);

        // Shader
        _cmdList->setShader(m_wireframeShaderKey);

        // Draw Alpha
        {
            root3D.color = float4(0, 1, 0, 0.125f);
            _cmdList->setInlineRootConstants(&root3D, RootConstants3DCount);

            // Blend (2)
            BlendState bsAlpha(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha, BlendOp::Add);
            _cmdList->setBlendState(bsAlpha);

            // DepthStencil (2)
            DepthStencilState dsAlpha(false, false, ComparisonFunc::Always);
            _cmdList->setDepthStencilState(dsAlpha);

            // Draw
            _cmdList->drawIndexed(m_unitBoxIB->getBufDesc().elementCount);
        }

        // Draw opaque
        {
            root3D.color = float4(0, 1, 0, 1.0f);
            _cmdList->setInlineRootConstants(&root3D, RootConstants3DCount);

            // Blend
            BlendState bsOpaque(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);
            _cmdList->setBlendState(bsOpaque);

            // DepthStencil
            DepthStencilState dsOpaque(true, true, ComparisonFunc::LessEqual);
            _cmdList->setDepthStencilState(dsOpaque);

            // Draw
            _cmdList->drawIndexed(m_unitBoxIB->getBufDesc().elementCount);
        }
    }

    //--------------------------------------------------------------------------------------
    void ForwardPass::drawGrid(CommandList * _cmdList, const float4x4 & _viewProj) const
    {
        RasterizerState rs(FillMode::Wireframe, CullMode::None);

        const BufferDesc & gridDesc = m_gridVB->getBufDesc();

        RootConstants3D root3D;

        root3D.mat = transpose(_viewProj);
        root3D.setBuffer(m_gridVB->getBindlessSRVHandle());
        root3D.setMode(MODE_VS_COLOR);
        root3D.color = float4(1, 1, 1, 1);

        _cmdList->setRasterizerState(rs);
        _cmdList->setShader(m_forwardShaderKey);
        _cmdList->setInlineRootConstants(&root3D, RootConstants3DCount);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::LineList);
        _cmdList->draw(gridDesc.elementCount);
    }

    //--------------------------------------------------------------------------------------
    void ForwardPass::drawAxis(CommandList * _cmdList, const float4x4 & _viewProj) const
    {
        RasterizerState rs(FillMode::Wireframe, CullMode::None);

        const BufferDesc & gridDesc = m_axisVB->getBufDesc();

        RootConstants3D root3D;

        root3D.mat = transpose(_viewProj);
        root3D.setBuffer(m_axisVB->getBindlessSRVHandle());
        root3D.setMode(MODE_VS_COLOR);
        root3D.color = float4(1, 1, 1, 1);

        _cmdList->setRasterizerState(rs);
        _cmdList->setShader(m_forwardShaderKey);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::LineList);

        _cmdList->setInlineRootConstants(&root3D, RootConstants3DCount);
        _cmdList->draw(6);
    }
}