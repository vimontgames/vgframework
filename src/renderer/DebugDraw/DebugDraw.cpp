#include "renderer/Precomp.h"
#include "DebugDraw.h"
#include "renderer/Geometry/Mesh/MeshGeometry.h"
#include "gfx/Device/Device.h"
#include "gfx/CommandList/CommandList.h"
#include "gfx/BindlessTable/BindlessTable.h"
#include "gfx/Resource/Buffer.h"
#include "gfx/PipelineState/Graphic/RasterizerState.h"

#include "Shaders/debugdraw/debugdraw.hlsli" 

using namespace vg::gfx;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    DebugDraw::DebugDraw() :
        m_debugDrawVBSize(16*1024)
    {
        auto * device = Device::get();
        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();

        RootSignatureDesc rsDesc;
        rsDesc.addRootConstants(ShaderStageFlags::All, 0, 0, DebugDrawRootConstants3DCount);
        rsDesc.addTable(bindlessTable);
        
        m_debugDrawSignatureHandle = device->addRootSignature(rsDesc);
        m_debugDrawShaderKey = ShaderKey("debugdraw/debugdraw.hlsl", "DebugDraw");

        createBoxPrimitive();
        createGrid();
        createAxis();
        createDebugDrawBuffer();
    }

    //--------------------------------------------------------------------------------------
    DebugDraw::~DebugDraw()
    {
        Device::get()->removeRootSignature(m_debugDrawSignatureHandle);

        VG_SAFE_RELEASE(m_debugDrawVB);
        VG_SAFE_RELEASE(m_box);
        VG_SAFE_RELEASE(m_gridVB);
        VG_SAFE_RELEASE(m_axisVB);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::createDebugDrawBuffer()
    {
        BufferDesc vbDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, m_debugDrawVBSize>>2, 4);
        m_debugDrawVB = Device::get()->createBuffer(vbDesc, "debugDrawVB");
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::createBoxPrimitive()
    {
        auto * device = Device::get();

        DebugDrawVertex vertices[8];

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

        BufferDesc vbDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(DefaultVertex), (uint)countof(vertices));
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
        Buffer * ib = device->createBuffer(ibDesc, "BoxIB", indices);

        m_box = new MeshGeometry("Box", this);
        m_box->setIndexBuffer(ib);
        m_box->setVertexBuffer(vb);
        m_box->addBatch(indiceCount);

        VG_SAFE_RELEASE(ib);
        VG_SAFE_RELEASE(vb);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::createGrid()
    {
        auto * device = Device::get();

        vector<DebugDrawVertex> vertices;

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

        BufferDesc vbDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(DebugDrawVertex), (u32)vertices.size());

        m_gridVB = device->createBuffer(vbDesc, "GridVB", vertices.data());
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::createAxis()
    {
        auto * device = Device::get();

        DebugDrawVertex vertices[6];

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

        BufferDesc vbDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(DebugDrawVertex), (uint)countof(vertices));

        m_axisVB = device->createBuffer(vbDesc, "AxisVB", vertices);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::drawAABB(CommandList * _cmdList, const AABB & _aabb, const float4x4 & _world) const
    {
        RasterizerState rs(FillMode::Wireframe, CullMode::None);
        _cmdList->setRasterizerState(rs);

        // AABB Matrix in world-space
        float4x4 aabbMatrixWS = mul(_aabb.getLocalSpaceUnitCubeMatrix(), _world);

        DebugDrawRootConstants3D root3D;
        {
            root3D.mat = transpose(aabbMatrixWS);
            root3D.setBufferHandle(m_box->getVertexBuffer()->getBufferHandle());
            root3D.setVertexBufferOffset(m_box->getVertexBufferOffset());
            root3D.setVertexFormat(VertexFormat::DebugDraw);
        }

        // Root sig
        _cmdList->setGraphicRootSignature(m_debugDrawSignatureHandle);

        // Geometry
        _cmdList->setIndexBuffer(m_box->getIndexBuffer());
        _cmdList->setPrimitiveTopology(PrimitiveTopology::LineList);

        // Shader
        _cmdList->setShader(m_debugDrawShaderKey);

        const auto & batches = m_box->batches();

        // Draw Alpha
        {
            root3D.color = float4(0, 1, 0, 0.125f);
            _cmdList->setGraphicRootConstants(0, (u32 *)&root3D, DebugDrawRootConstants3DCount);

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
            _cmdList->setGraphicRootConstants(0, (u32 *)&root3D, DebugDrawRootConstants3DCount);

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
    void DebugDraw::drawGrid(CommandList * _cmdList) const
    {
        RasterizerState rs(FillMode::Wireframe, CullMode::None);

        VG_ASSERT(nullptr != m_gridVB);
        const BufferDesc & gridDesc = m_gridVB->getBufDesc();

        DebugDrawRootConstants3D root3D;

        root3D.mat = /*transpose*/(float4x4::identity());
        root3D.setBufferHandle(m_gridVB->getBufferHandle());
        root3D.setVertexFormat(VertexFormat::DebugDraw);
        root3D.color = float4(1, 1, 1, 1);

        _cmdList->setRasterizerState(rs);
        _cmdList->setGraphicRootSignature(m_debugDrawSignatureHandle);
        _cmdList->setShader(m_debugDrawShaderKey);
        _cmdList->setGraphicRootConstants(0, (u32 *)&root3D, DebugDrawRootConstants3DCount);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::LineList);
        _cmdList->draw(gridDesc.elementCount);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::drawAxis(CommandList * _cmdList) const
    {
        RasterizerState rs(FillMode::Wireframe, CullMode::None);

        VG_ASSERT(nullptr != m_axisVB);
        const BufferDesc & gridDesc = m_axisVB->getBufDesc();

        DebugDrawRootConstants3D root3D;

        root3D.mat = /*transpose*/(float4x4::identity());
        root3D.setBufferHandle(m_axisVB->getBufferHandle());
        root3D.setVertexFormat(VertexFormat::DebugDraw);
        root3D.color = float4(1, 1, 1, 1);

        _cmdList->setRasterizerState(rs);
        _cmdList->setShader(m_debugDrawShaderKey);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::LineList);

        _cmdList->setGraphicRootConstants(0, (u32 *)&root3D, DebugDrawRootConstants3DCount);
        _cmdList->draw(6);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::AddLine(const core::float3 & _beginPos, const core::float3 & _endPos, core::u32 _color)
    {
        DebugDrawLineData & line = m_lines.push_empty();
        line.beginPos = _beginPos;
        line.beginColor = _color;
        line.endPos = _endPos;
        line.endColor = _color;
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::update(gfx::CommandList * _cmdList)
    {
        uint_ptr offset = 0;

        uint_ptr lineStartOffset = offset;
        uint lineCount = 0;

        u8 * dbgDrawData = (u8 *)_cmdList->map(m_debugDrawVB).data;
        {
            for (uint i = 0; i < m_lines.size(); ++i)
            {
                VG_ASSERT(uint_ptr(offset + 2 * sizeof(DebugDrawVertex)) < m_debugDrawVBSize);

                if (uint_ptr(offset + 2 * sizeof(DebugDrawVertex)) < m_debugDrawVBSize)
                {
                    const auto & line = m_lines[i];
                    DebugDrawVertex * v0 = ((DebugDrawVertex *)(dbgDrawData + offset));
                    memcpy(v0->pos, &line.beginPos, sizeof(float) * 3);
                    v0->color = line.beginColor;
                    offset += sizeof(DebugDrawVertex);

                    DebugDrawVertex * v1 = ((DebugDrawVertex *)(dbgDrawData + offset));
                    memcpy(v1->pos, &line.endPos, sizeof(float) * 3);
                    v1->color = line.endColor;
                    offset += sizeof(DebugDrawVertex);

                    lineCount++;
                }
            }
        }
        _cmdList->unmap(m_debugDrawVB, dbgDrawData);

        m_linesVBOffset = (u32)lineStartOffset;
        m_linesToDraw = lineCount;
        m_lines.clear();
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::render(gfx::CommandList * _cmdList)
    {
        // draw lines
        if (m_linesToDraw > 0)
        {
            RasterizerState rs(FillMode::Wireframe, CullMode::None);

            VG_ASSERT(nullptr != m_debugDrawVB);

            // Root sig
            _cmdList->setGraphicRootSignature(m_debugDrawSignatureHandle);

            // Root constants
            DebugDrawRootConstants3D root3D;
            root3D.mat = /*transpose*/(float4x4::identity());
            root3D.setBufferHandle(m_debugDrawVB->getBufferHandle());
            root3D.setVertexBufferOffset(m_linesVBOffset);
            root3D.setVertexFormat(VertexFormat::DebugDraw);
            root3D.color = float4(1, 1, 1, 1);

            _cmdList->setRasterizerState(rs);
            _cmdList->setShader(m_debugDrawShaderKey);
            _cmdList->setPrimitiveTopology(PrimitiveTopology::LineList);

            _cmdList->setGraphicRootConstants(0, (u32 *)&root3D, DebugDrawRootConstants3DCount);
            _cmdList->draw(m_linesToDraw<<1);

            m_linesToDraw = 0;
        }
    }
}