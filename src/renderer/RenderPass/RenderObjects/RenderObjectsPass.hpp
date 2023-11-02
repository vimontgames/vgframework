#include "RenderObjectsPass.h"
#include "renderer/Geometry/AABB/AABB.h"
#include "renderer/Geometry/Mesh/MeshGeometry.h"
#include "renderer/Job/Culling/GraphicInstanceList.h"
#include "renderer/View/View.h"
#include "renderer/IGraphicInstance.h"

#include "shaders/system/vertex.hlsl.h"
#include "shaders/editor/editor.hlsli"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    RenderObjectsPass::RenderObjectsPass(const core::string & _name) :
        RenderPass(_name)
    {
        setUserPassType(RenderPassType::Graphic);

        auto * device = Device::get();
        auto * renderer = Renderer::get();

        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();

        RootSignatureDesc rsDesc;
        rsDesc.addRootConstants(ShaderStageFlags::All, 0, 0, EditorRootConstants3DCount);
        rsDesc.addTable(bindlessTable);

        m_debugDrawSignatureHandle = device->addRootSignature(rsDesc);
        m_debugDrawShaderKey = ShaderKey("editor/editor.hlsl", "DebugDraw");

        // TODO: create once and share
        createGrid();
        createAxis();
        createUnitBox(); // used to draw AABB
    }

    //--------------------------------------------------------------------------------------
    RenderObjectsPass::~RenderObjectsPass()
    {
        destroyUnitBox();
        destroyAxis();
        destroyGrid();

        auto * device = Device::get();
        device->removeRootSignature(m_debugDrawSignatureHandle);
    }

    //--------------------------------------------------------------------------------------
    void RenderObjectsPass::createUnitBox()
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
        Buffer * vb = device->createBuffer(vbDesc, "unitBoxVB", vertices);

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

        const uint indiceCount = (uint)countof(indices);
        BufferDesc ibDesc(Usage::Default, BindFlags::IndexBuffer, CPUAccessFlags::None, BufferFlags::None, sizeof(u16), indiceCount);
        Buffer * ib = device->createBuffer(ibDesc, "unitBoxIB", indices);

        m_unitBox = new MeshGeometry("UnitBox", this);
        m_unitBox->setIndexBuffer(ib);
        m_unitBox->setVertexBuffer(vb);
        m_unitBox->addBatch(indiceCount);

        VG_SAFE_RELEASE(ib);
        VG_SAFE_RELEASE(vb);
    }

    //--------------------------------------------------------------------------------------
    void RenderObjectsPass::destroyUnitBox()
    {
        VG_SAFE_RELEASE(m_unitBox);
    }

    //--------------------------------------------------------------------------------------
    void RenderObjectsPass::drawAABB(CommandList * _cmdList, const AABB & _aabb, const float4x4 & _world, const float4x4 & _viewProj) const
    {
        RasterizerState rs(FillMode::Wireframe, CullMode::None);
        _cmdList->setRasterizerState(rs);

        // AABB Matrix in world-space
        float4x4 aabbMatrixWS = mul(_aabb.getLocalSpaceUnitCubeMatrix(), _world);
        float4x4 wvp = mul(aabbMatrixWS, _viewProj);

        EditorRootConstants3D root3D;
        {
            root3D.mat = transpose(wvp);
            root3D.setBufferHandle(m_unitBox->getVertexBuffer()->getBufferHandle());
            root3D.setVertexBufferOffset(m_unitBox->getVertexBufferOffset());
        }

        // Root sig
        _cmdList->setGraphicRootSignature(m_debugDrawSignatureHandle);

        // Geometry
        _cmdList->setIndexBuffer(m_unitBox->getIndexBuffer());
        _cmdList->setPrimitiveTopology(PrimitiveTopology::LineList);

        // Shader
        _cmdList->setShader(m_debugDrawShaderKey);

        const auto & batches = m_unitBox->batches();

        // Draw Alpha
        {
            root3D.color = float4(0, 1, 0, 0.125f);
            _cmdList->setGraphicRootConstants(0, (u32*)&root3D, EditorRootConstants3DCount);

            // Blend (2)
            BlendState bsAlpha(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha, BlendOp::Add);
            _cmdList->setBlendState(bsAlpha);

            // DepthStencil (2)
            DepthStencilState dsAlpha(false, false, ComparisonFunc::Always);
            _cmdList->setDepthStencilState(dsAlpha);

            // Draw
            for (uint i = 0; i < batches.size(); ++i)
                _cmdList->drawIndexed(batches[i].count);
        }

        // Draw opaque
        {
            root3D.color = float4(0, 1, 0, 1.0f);
            _cmdList->setGraphicRootConstants(0, (u32*)&root3D, EditorRootConstants3DCount);

            // Blend
            BlendState bsOpaque(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);
            _cmdList->setBlendState(bsOpaque);

            // DepthStencil
            DepthStencilState dsOpaque(true, true, ComparisonFunc::LessEqual);
            _cmdList->setDepthStencilState(dsOpaque);

            // Draw
            for (uint i = 0; i < batches.size(); ++i)
                _cmdList->drawIndexed(batches[i].count);
        }
    }

    //--------------------------------------------------------------------------------------
    void RenderObjectsPass::createGrid()
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
            v1.setPos({ (float)i, (float)end, 0.0f });
            v1.setColor(0xFF0D0D0D);

            auto & h0 = vertices.emplace_back();
            h0.setPos({ (float)begin,(float)i, 0.0f });
            h0.setColor(0xFF0D0D0D);

            auto & h1 = vertices.emplace_back();
            h1.setPos({ (float)end,(float)i, 0.0f });
            h1.setColor(0xFF0D0D0D);
        }

        BufferDesc vbDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(SimpleVertex), (u32)vertices.size());

        m_gridVB = device->createBuffer(vbDesc, "GridVB", vertices.data());
    }

    //--------------------------------------------------------------------------------------
    void RenderObjectsPass::destroyGrid()
    {
        VG_SAFE_RELEASE(m_gridVB);
    }

    //--------------------------------------------------------------------------------------
    void RenderObjectsPass::drawGrid(CommandList * _cmdList, const float4x4 & _viewProj) const
    {
        RasterizerState rs(FillMode::Wireframe, CullMode::None);

        const BufferDesc & gridDesc = m_gridVB->getBufDesc();

        EditorRootConstants3D root3D;

        root3D.mat = transpose(_viewProj);
        root3D.setBufferHandle(m_gridVB->getBufferHandle());
        root3D.color = float4(1, 1, 1, 1);

        _cmdList->setRasterizerState(rs);
        _cmdList->setGraphicRootSignature(m_debugDrawSignatureHandle);
        _cmdList->setShader(m_debugDrawShaderKey);
        _cmdList->setGraphicRootConstants(0, (u32*)&root3D, EditorRootConstants3DCount);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::LineList);
        _cmdList->draw(gridDesc.elementCount);
    }

    //--------------------------------------------------------------------------------------
    void RenderObjectsPass::createAxis()
    {
        auto * device = Device::get();

        SimpleVertex vertices[6];

        const float eps = 0.0001f;

        vertices[0].setPos({ 0.0f, 0.0f, eps + 0.0f });
        vertices[0].setColor(0xFF0000FF);
        vertices[1].setPos({ 1.0f, 0.0f, eps + 0.0f });
        vertices[1].setColor(0xFF0000FF);

        vertices[2].setPos({ 0.0f, 0.0f, eps + 0.0f });
        vertices[2].setColor(0xFF00FF00);
        vertices[3].setPos({ 0.0f, 1.0f, eps + 0.0f });
        vertices[3].setColor(0xFF00FF00);

        vertices[4].setPos({ 0.0f, 0.0f, eps + 0.0f });
        vertices[4].setColor(0xFFFF0000);
        vertices[5].setPos({ 0.0f, 0.0f, eps + 1.0f });
        vertices[5].setColor(0xFFFF0000);

        BufferDesc vbDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(SimpleVertex), (uint)countof(vertices));

        m_axisVB = device->createBuffer(vbDesc, "AxisVB", vertices);
    }

    //--------------------------------------------------------------------------------------
    void RenderObjectsPass::destroyAxis()
    {
        VG_SAFE_RELEASE(m_axisVB);
    }

    //--------------------------------------------------------------------------------------
    void RenderObjectsPass::drawAxis(CommandList * _cmdList, const float4x4 & _viewProj) const
    {
        RasterizerState rs(FillMode::Wireframe, CullMode::None);

        const BufferDesc & gridDesc = m_axisVB->getBufDesc();

        EditorRootConstants3D root3D;

        root3D.mat = transpose(_viewProj);
        root3D.setBufferHandle(m_axisVB->getBufferHandle());
        root3D.color = float4(1, 1, 1, 1);

        _cmdList->setRasterizerState(rs);
        _cmdList->setShader(m_debugDrawShaderKey);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::LineList);

        _cmdList->setGraphicRootConstants(0, (u32*)&root3D, EditorRootConstants3DCount);
        _cmdList->draw(6);
    }

    //--------------------------------------------------------------------------------------
    void RenderObjectsPass::DrawGraphicInstances(const RenderContext & _renderContext, CommandList * _cmdList, const GraphicInstanceList & _graphicInstancesList) const
    {
        const auto & list = _graphicInstancesList.m_instances;

        for (uint i = 0; i < list.size(); ++i)
        {
            const IGraphicInstance * instance = list[i];
            instance->Draw(_renderContext, _cmdList);
        }
    }
}