#include "renderer/Precomp.h"
#include "DebugDraw.h"
#include "core/IScheduler.h"
#include "core/IWorld.h"
#include "engine/IEngine.h"
#include "renderer/Renderer.h"
#include "renderer/Geometry/Mesh/MeshGeometry.h"
#include "gfx/Device/Device.h"
#include "gfx/CommandList/CommandList.h"
#include "gfx/BindlessTable/BindlessTable.h"
#include "gfx/Resource/Buffer.h"
#include "gfx/PipelineState/Graphic/RasterizerState.h"

#include "Shaders/debugdraw/debugdraw.hlsli" 

using namespace vg::core;
using namespace vg::gfx;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    DebugDraw::WorldData::WorldData() :
        m_lines(1024 * 16)
    {
    }

    //--------------------------------------------------------------------------------------
    DebugDraw::WorldData::~WorldData()
    {

    }

    //--------------------------------------------------------------------------------------
    DebugDraw::DebugDraw() 
    {
        auto * device = Device::get();
        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();

        RootSignatureDesc rsDesc;
        rsDesc.addRootConstants(ShaderStageFlags::All, 0, 0, DebugDrawRootConstants3DCount);
        rsDesc.addTable(bindlessTable);
        
        m_debugDrawSignatureHandle = device->addRootSignature(rsDesc);
        m_debugDrawShaderKey = ShaderKey("debugdraw/debugdraw.hlsl", "DebugDraw");

        createBoxPrimitive();
        createIcoSpherePrimitive();
        createGrid();
        createAxis();
        createCylinderPrimitive();
    }

    //--------------------------------------------------------------------------------------
    DebugDraw::~DebugDraw()
    {
        Device::get()->removeRootSignature(m_debugDrawSignatureHandle);

        clearDrawData();
        clearWorldData();   

        VG_SAFE_RELEASE(m_box);
        VG_SAFE_RELEASE(m_gridVB);
        VG_SAFE_RELEASE(m_axisVB);
        VG_SAFE_RELEASE(m_icoSphere);
        VG_SAFE_RELEASE(m_hemiSphere);
        VG_SAFE_RELEASE(m_cylinder);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::clearWorldData()
    {
        //for (auto & pair : m_worldData)
        //{
        //    WorldData & worldData = pair.second;
        //
        //    // TODO
        //}
        //m_worldData.clear();
    }

    //--------------------------------------------------------------------------------------
    DebugDraw::WorldData * DebugDraw::getWorldData(const core::IWorld * _world)
    {
        if (IDebugDrawData * data = _world->GetDebugDrawData())
        {
            return (DebugDraw::WorldData *)data;
        }
        else
        {
            auto * newData = new DebugDraw::WorldData();
            ((core::IWorld*)(_world))->SetDebugDrawData(newData);
            return newData;
        }
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::clearDrawData()
    {
        for (auto & pair : m_drawData)
        {
            DrawData & drawData = pair.second;
            VG_SAFE_RELEASE_ASYNC(drawData.m_debugDrawVB);
        }
        m_drawData.clear();
    }

    //--------------------------------------------------------------------------------------
    DebugDraw::DrawData & DebugDraw::getDrawData(const View * _view)
    {
        auto it = m_drawData.find(_view);

        if (it == m_drawData.end())
        {
            DrawData drawData;
            drawData.m_debugDrawVBSize = 1024 * 1024;

            BufferDesc vbDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, drawData.m_debugDrawVBSize >> 2, 4);
            drawData.m_debugDrawVB = Device::get()->createBuffer(vbDesc, "debugDrawVB");

            m_drawData.insert(std::pair(_view, drawData));
            it = m_drawData.find(_view);
        }

        return it->second;
    }

    using VertexList = std::vector<DebugDrawVertex>;
    struct Triangle
    {
        Triangle(u16 _indice0 = -1, u16 _indice1 = -1, u16 _indice2 = -1)
        {
            indices[0] = _indice0;
            indices[1] = _indice1;
            indices[2] = _indice2;
        }

        u16 indices[3];
    };

    using TriangleList = std::vector<Triangle>;

    //--------------------------------------------------------------------------------------
    // Inspired by method used in https://schneide.blog/2016/07/15/generating-an-icosphere-in-c/
    //--------------------------------------------------------------------------------------
    void DebugDraw::createIcoSpherePrimitive()
    {
        // Start with a hard-coded indexed-mesh representation of the icosahedron
        const float X = 0.525731112119133606f;
        const float Z = 0.850650808352039932f;
        const float N = 0.0f;

        static VertexList vertices =
        {
            {-X,N,Z}, { X,N, Z}, {-X,N,-Z}, { X, N,-Z},
            { N,Z,X}, { N,Z,-X}, { N,-Z,X}, { N,-Z,-X},
            { Z,X,N}, {-Z,X, N}, { Z,-X,N}, {-Z,-X, N}
        };

        static TriangleList triangles =
        {
          {0,4,1},{0,9,4},{9,5,4},{4,5,8},{4,8,1},
          {8,10,1},{8,3,10},{5,3,8},{5,2,3},{2,7,3},
          {7,10,3},{7,6,10},{7,11,6},{11,0,6},{0,1,6},
          {6,1,10},{9,0,11},{9,11,2},{9,2,5},{7,2,11}
        };
     
        using Lookup = map<std::pair<u16, u16>, u16>;
        auto vertex_for_edge = [](Lookup & _lookup, VertexList & _vertices, u16 _first, u16 _second) 
        {
            Lookup::key_type key(_first, _second);
            if (key.first > key.second)
                std::swap(key.first, key.second);
            
            auto inserted = _lookup.insert({ key, (u16)_vertices.size() });
            if (inserted.second)
            {
                auto & edge0 = _vertices[_first];
                auto & edge1 = _vertices[_second];
                float3 point = normalize( float3(edge0.pos[0], edge0.pos[1], edge0.pos[2]) + float3(edge1.pos[0], edge1.pos[1], edge1.pos[2]));
                _vertices.push_back(DebugDrawVertex(point.x, point.y, point.z));
            }
            
            return (u16)inserted.first->second;
        };

        auto subdivide = [=](VertexList & _vertices, TriangleList _triangles)
        {
            Lookup lookup;
            TriangleList result;
            result.reserve(_triangles.size() * 4);

            for (auto && each : _triangles)
            {
                u16 mid[3];
                for (int edge = 0; edge < 3; ++edge)
                    mid[edge] = vertex_for_edge(lookup, _vertices, each.indices[edge], each.indices[(edge + 1) % 3]);

                result.push_back({ each.indices[0], mid[0], mid[2] });
                result.push_back({ each.indices[1], mid[1], mid[0] });
                result.push_back({ each.indices[2], mid[2], mid[1] });
                result.push_back({ mid[0], mid[1], mid[2] });
            }

            return result;
        };
        
        for (int i = 0; i < 2; ++i)
            triangles = subdivide(vertices, triangles);

        auto * device = Device::get();

        // Create full sphere
        {
            BufferDesc vbDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(DebugDrawVertex), (uint)vertices.size());
            Buffer * vb = device->createBuffer(vbDesc, "IcoSphereVB", vertices.data());

            const uint indiceCount = (uint)triangles.size() * 3;
            BufferDesc ibDesc(Usage::Default, BindFlags::IndexBuffer, CPUAccessFlags::None, BufferFlags::None, sizeof(u16), indiceCount);
            Buffer * ib = device->createBuffer(ibDesc, "IcoSphereIB", triangles.data());

            m_icoSphere = new MeshGeometry("IcoSphere", this);
            m_icoSphere->setIndexBuffer(ib);
            m_icoSphere->setVertexBuffer(vb);
            m_icoSphere->addBatch(indiceCount);

            VG_SAFE_RELEASE(ib);
            VG_SAFE_RELEASE(vb);
        }

        // Create Hemisphere by discarding triangles z < 0 and clamping
        {
            VertexList hemiVertices;
            TriangleList hemiTriangles;

            auto pushVertex = [](VertexList & hemiVertices, DebugDrawVertex & _vertex)
            {
                if (_vertex.pos[2] < 0.0f)
                {
                    float3 pos = float3(_vertex.pos[0], _vertex.pos[1], _vertex.pos[2]);
                    pos.z = 0.0f;
                    pos = normalize(pos);
                    _vertex.pos[0] = pos.x;
                    _vertex.pos[1] = pos.y;
                    _vertex.pos[2] = pos.z;
                }

                const uint count = (uint)hemiVertices.size();
                for (uint j = 0; j < count; ++j)
                {
                    if (!memcmp(&hemiVertices[j], &_vertex, sizeof(DebugDrawVertex)))
                        return j;
                }

                hemiVertices.push_back(_vertex);
                return count;
            };
            
            for (uint i = 0; i < triangles.size(); ++i)
            {
                auto & v0 = vertices[triangles[i].indices[0]];
                auto & v1 = vertices[triangles[i].indices[1]];
                auto & v2 = vertices[triangles[i].indices[2]];

                if (v0.pos[2] >= 0.0f || v1.pos[2] >= 0.0f || v2.pos[2] >= 0.0f)
                {
                    Triangle tri;
                    tri.indices[0] = pushVertex(hemiVertices, v0);
                    tri.indices[1] = pushVertex(hemiVertices, v1);
                    tri.indices[2] = pushVertex(hemiVertices, v2);

                    hemiTriangles.push_back(tri);
                }
            }

            BufferDesc vbDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(DebugDrawVertex), (uint)hemiVertices.size());
            Buffer * vb = device->createBuffer(vbDesc, "HemiSphereVB", hemiVertices.data());

            const uint indiceCount = (uint)hemiTriangles.size() * 3;
            BufferDesc ibDesc(Usage::Default, BindFlags::IndexBuffer, CPUAccessFlags::None, BufferFlags::None, sizeof(u16), indiceCount);
            Buffer * ib = device->createBuffer(ibDesc, "HemiSphereIB", hemiTriangles.data());

            m_hemiSphere = new MeshGeometry("HemiSphere", this);
            m_hemiSphere->setIndexBuffer(ib);
            m_hemiSphere->setVertexBuffer(vb);
            m_hemiSphere->addBatch(indiceCount);

            VG_SAFE_RELEASE(ib);
            VG_SAFE_RELEASE(vb);
        }
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::createCylinderPrimitive()
    {
        VertexList vertices;
        TriangleList triangles;

        uint resolution = 16;
        float radius = 1.0f;
        float height = 1.0f;

        vertices.reserve(resolution * 2);
        triangles.reserve(resolution * 4);

        auto pushVertex = [](VertexList & vertices, DebugDrawVertex & _vertex)
        {
            const uint count = (uint)vertices.size();
            for (uint j = 0; j < count; ++j)
            {
                if (!memcmp(&vertices[j], &_vertex, sizeof(DebugDrawVertex)))
                    return j;
            }

            vertices.push_back(_vertex);
            return count;
        };

        // generate bottom vertices
        u16 firstBot, firstTop;
        u16 prevBot, prevTop;
        for (size_t i = 0; i < resolution; i++) 
        {
            float ratio = (float)i / (float)resolution;
            float r = ratio * (PI * 2.0f);
            float x = cos(r) * radius;
            float y = sin(r) * radius;

            auto bottom = DebugDrawVertex(x, y, -height);
            auto top = DebugDrawVertex(x, y, height);

            u16 vbot = pushVertex(vertices, bottom);
            u16 vtop = pushVertex(vertices, top);

            if (i == 0)
            {
                firstBot = vbot;
                firstTop = vtop;
            }
            else // if (i > 0)
            {
                triangles.push_back({ prevTop, vtop, prevBot });
                triangles.push_back({ prevBot, vbot, vtop });
            }

            prevBot = vbot;
            prevTop = vtop;
        }

        triangles.push_back({ firstTop, prevTop, firstBot });
        triangles.push_back({ firstBot, prevBot, prevTop });

        auto * device = Device::get();

        BufferDesc vbDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(DebugDrawVertex), (uint)vertices.size());
        Buffer * vb = device->createBuffer(vbDesc, "CylinderVB", vertices.data());

        const uint indiceCount = (uint)triangles.size() * 3;
        BufferDesc ibDesc(Usage::Default, BindFlags::IndexBuffer, CPUAccessFlags::None, BufferFlags::None, sizeof(u16), indiceCount);
        Buffer * ib = device->createBuffer(ibDesc, "CylinderIB", triangles.data());

        m_cylinder = new MeshGeometry("Cylinder", this);
        m_cylinder->setIndexBuffer(ib);
        m_cylinder->setVertexBuffer(vb);
        m_cylinder->addBatch(indiceCount);

        VG_SAFE_RELEASE(ib);
        VG_SAFE_RELEASE(vb);
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

        const uint gridSize = 64;

        int begin = -(int)gridSize / 2;
        int end = (int)gridSize / 2;

        vector<DebugDrawVertex> vertices;
        vertices.reserve((end - begin + 1) << 2);

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

        DebugDrawRootConstants3D debugDraw3D;
        {
            debugDraw3D.setWorldMatrix(transpose(aabbMatrixWS));
            debugDraw3D.setBufferHandle(m_box->getVertexBuffer()->getBufferHandle(), m_box->getVertexBufferOffset());
            debugDraw3D.setVertexFormat(VertexFormat::DebugDraw);
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
            debugDraw3D.color = float4(0, 1, 0, 0.125f);
            _cmdList->setGraphicRootConstants(0, (u32 *)&debugDraw3D, DebugDrawRootConstants3DCount);

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
            debugDraw3D.color = float4(0, 1, 0, 1.0f);
            _cmdList->setGraphicRootConstants(0, (u32 *)&debugDraw3D, DebugDrawRootConstants3DCount);

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

        DebugDrawRootConstants3D debugDrawRoot3D;

        debugDrawRoot3D.setWorldMatrix(float4x4::identity());
        debugDrawRoot3D.setBufferHandle(m_gridVB->getBufferHandle());
        debugDrawRoot3D.setVertexFormat(VertexFormat::DebugDraw);
        debugDrawRoot3D.setColor(float4(1, 1, 1, 1));

        _cmdList->setRasterizerState(rs);
        _cmdList->setGraphicRootSignature(m_debugDrawSignatureHandle);
        _cmdList->setShader(m_debugDrawShaderKey);
        _cmdList->setGraphicRootConstants(0, (u32 *)&debugDrawRoot3D, DebugDrawRootConstants3DCount);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::LineList);
        _cmdList->draw(gridDesc.elementCount);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::drawAxis(CommandList * _cmdList) const
    {
        RasterizerState rs(FillMode::Wireframe, CullMode::None);

        VG_ASSERT(nullptr != m_axisVB);
        const BufferDesc & gridDesc = m_axisVB->getBufDesc();

        DebugDrawRootConstants3D debugDrawRoot3D;

        debugDrawRoot3D.setWorldMatrix(float4x4::identity());
        debugDrawRoot3D.setBufferHandle(m_axisVB->getBufferHandle());
        debugDrawRoot3D.setVertexFormat(VertexFormat::DebugDraw);
        debugDrawRoot3D.setColor(float4(1, 1, 1, 1));

        _cmdList->setRasterizerState(rs);
        _cmdList->setShader(m_debugDrawShaderKey);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::LineList);

        _cmdList->setGraphicRootConstants(0, (u32 *)&debugDrawRoot3D, DebugDrawRootConstants3DCount);
        _cmdList->draw(6);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::AddLine(const core::IWorld * _world, const core::float3 & _beginPos, const core::float3 & _endPos, core::u32 _color, const core::float4x4 & _matrix)
    {
        addLine(_world, _beginPos, _endPos, _color, _matrix);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::addLine(const core::IWorld * _world, const core::float3 & _beginPos, const core::float3 & _endPos, core::u32 _color, const core::float4x4 & _matrix)
    {
        DebugDrawLineData & line = getWorldData(_world)->m_lines.push_empty_atomic();

        line.world = _matrix;
        line.beginPos = _beginPos;
        line.beginColor = _color;
        line.endPos = _endPos;
        line.endColor = _color;
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::AddWireframeBox(const core::IWorld * _world, const float3 & _minPos, const float3 & _maxPos, u32 _color, const float4x4 & _matrix)
    {
        DebugDrawLineData * lines = getWorldData(_world)->m_lines.alloc(12);

        lines[0x0] = { _matrix, float3(_minPos.x, _minPos.y, _minPos.z), _color, float3(_maxPos.x, _minPos.y, _minPos.z), _color };
        lines[0x1] = { _matrix, float3(_maxPos.x, _minPos.y, _minPos.z), _color, float3(_maxPos.x, _maxPos.y, _minPos.z), _color };
        lines[0x2] = { _matrix, float3(_maxPos.x, _maxPos.y, _minPos.z), _color, float3(_minPos.x, _maxPos.y, _minPos.z), _color };
        lines[0x3] = { _matrix, float3(_minPos.x, _maxPos.y, _minPos.z), _color, float3(_minPos.x, _minPos.y, _minPos.z), _color };
                                                            
        lines[0x4] = { _matrix, float3(_minPos.x, _minPos.y, _minPos.z), _color, float3(_minPos.x, _minPos.y, _maxPos.z), _color };
        lines[0x5] = { _matrix, float3(_maxPos.x, _minPos.y, _minPos.z), _color, float3(_maxPos.x, _minPos.y, _maxPos.z), _color };
        lines[0x6] = { _matrix, float3(_maxPos.x, _maxPos.y, _minPos.z), _color, float3(_maxPos.x, _maxPos.y, _maxPos.z), _color };
        lines[0x7] = { _matrix, float3(_minPos.x, _maxPos.y, _minPos.z), _color, float3(_minPos.x, _maxPos.y, _maxPos.z), _color };
                                                          
        lines[0x8] = { _matrix, float3(_minPos.x, _minPos.y, _maxPos.z), _color, float3(_maxPos.x, _minPos.y, _maxPos.z), _color };
        lines[0x9] = { _matrix, float3(_maxPos.x, _minPos.y, _maxPos.z), _color, float3(_maxPos.x, _maxPos.y, _maxPos.z), _color };
        lines[0xA] = { _matrix, float3(_maxPos.x, _maxPos.y, _maxPos.z), _color, float3(_minPos.x, _maxPos.y, _maxPos.z), _color };
        lines[0xB] = { _matrix, float3(_minPos.x, _maxPos.y, _maxPos.z), _color, float3(_minPos.x, _minPos.y, _maxPos.z), _color };
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::AddWireframeSphere(const core::IWorld * _world, const float _radius, core::u32 _color, const core::float4x4 _matrix)
    {
        DebugDrawIcoSphereData & icoSphere = getWorldData(_world)->m_icoSpheres.push_empty();
        icoSphere.world = mul(float4x4::scale(_radius), _matrix);
        icoSphere.color = _color;
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::AddHemisphere(const core::IWorld * _world, const float _radius, core::u32 _color, const core::float4x4 _matrix)
    {
        DebugDrawHemiSphereData & hemiSphere = getWorldData(_world)->m_hemiSpheres.push_empty();
        hemiSphere.world = mul(float4x4::scale(_radius), _matrix);
        hemiSphere.color = _color;
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::AddCylinder(const core::IWorld * _world, float _radius, float _height, core::u32 _color, const core::float4x4 _matrix)
    {
        DebugDrawCylinderData & cylinder = getWorldData(_world)->m_cylinders.push_empty();

        float3 s = float3(_radius, _radius, _height * 0.5f);

        const float4x4 scale = float4x4
        (
            s.x,   0,   0, 0,
              0, s.y,   0, 0,
              0,   0, s.z, 0,
              0,   0,   0, 1 
        );

        cylinder.world = mul(scale, _matrix);
        cylinder.color = _color;
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::AddCapsule(const core::IWorld * _world, float _radius, float _height, core::u32 _color, const core::float4x4 _matrix)
    {
        float offset = max(0.0f, _height - _radius * 2.0f);

        float4x4 topHemi = float4x4::identity();
        topHemi[3].xyz = float3(0, 0, 0.5f * offset);
        AddHemisphere(_world, _radius, _color, mul(topHemi, _matrix));

        AddCylinder(_world, _radius, offset, _color, _matrix);

        float4x4 bottomHemi = float4x4::identity();
        bottomHemi[2].xyz *= -1.0f;
        bottomHemi[3].xyz = float3(0, 0, -0.5f * offset);
        AddHemisphere(_world, _radius, _color, mul(bottomHemi, _matrix));
    }

    //--------------------------------------------------------------------------------------
    const vg::engine::IEngine * getEngine()
    {
        const auto * factory = Kernel::getFactory();
        return (const vg::engine::IEngine *)factory->getSingleton("Engine");
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::endFrame()
    {
        Renderer * renderer = Renderer::get();

        auto & worlds = getEngine()->GetWorlds();

        for (auto * world : worlds)
        {
            auto * worldData = (WorldData *)world->GetDebugDrawData();

            if (worldData)
            {
                worldData->m_lines.clear();
                worldData->m_icoSpheres.clear();
                worldData->m_hemiSpheres.clear();
                worldData->m_cylinders.clear();
            }
        }

        //set<IWorld *> worlds;
        //for (uint j = 0; j < core::enumCount<gfx::ViewTarget>(); ++j)
        //{
        //    auto target = (gfx::ViewTarget)j;
        //    auto & views = renderer->GetViews(target);
        //    for (uint i = 0; i < views.count(); ++i)
        //    {
        //        auto * view = views[i];
        //        if (view->IsVisible())
        //        {
        //            if (auto * world = view->GetWorld())
        //                worlds.insert(world);
        //        }
        //    }
        //}
        //
        //for (auto * world : worlds)
        //{
        //    auto * worldData = (WorldData *)world->GetDebugDrawData();
        //
        //    if (worldData)
        //    {
        //        worldData->m_lines.clear();
        //        worldData->m_icoSpheres.clear();
        //        worldData->m_hemiSpheres.clear();
        //        worldData->m_cylinders.clear();
        //    }
        //}


    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::reset()
    {
        clearDrawData();
        clearWorldData();
    }

    const float4 opaqueColor = float4(1, 1, 1, 0.2f);
    const float4 transparentColor = float4(0.5f, 0.5f, 0.5f, 0.1f);

    //--------------------------------------------------------------------------------------
    void DebugDraw::update(const View * _view, gfx::CommandList * _cmdList)
    {
        VG_PROFILE_CPU("DebugDrawUpdate");

        // Wait DebugDraw jobs
        {
            VG_PROFILE_CPU("Wait DebugDraw");
            auto scheduler = Kernel::getScheduler();
            scheduler->Wait(Renderer::get()->GetJobSync(IRenderer::JobSync::DebugDraw));
        }

        DrawData & drawData = getDrawData(_view);

        // reset
        drawData.m_linesToDraw = 0;
        //drawData.m_wireframeBoxesToDraw = 0;

        uint_ptr offset = 0;

        uint_ptr lineStartOffset = offset;
        uint lineCount = 0;

        auto world = ((IView *)_view)->GetWorld();
        if (!world)
            return;

        auto * worldData = getWorldData(world);

        const auto mapSizeInBytes = worldData->m_lines.size() * 2 * sizeof(DebugDrawVertex);
        if (mapSizeInBytes > 0)
        {
            u8 * dbgDrawData = (u8 *)_cmdList->map(drawData.m_debugDrawVB, mapSizeInBytes).data;
            {
                for (uint i = 0; i < worldData->m_lines.size(); ++i)
                {
                    VG_ASSERT(offset + 2 * sizeof(DebugDrawVertex) < drawData.m_debugDrawVBSize);

                    if (uint_ptr(offset + 2 * sizeof(DebugDrawVertex)) < drawData.m_debugDrawVBSize)
                    {
                        const auto & line = worldData->m_lines[i];

                        DebugDrawVertex * v0 = ((DebugDrawVertex *)(dbgDrawData + offset));
                        float3 pos0 = mul(float4(line.beginPos.xyz, 1), line.world).xyz;
                        memcpy(v0->pos, &pos0, sizeof(float) * 3);
                        v0->color = line.beginColor;
                        offset += sizeof(DebugDrawVertex);

                        DebugDrawVertex * v1 = ((DebugDrawVertex *)(dbgDrawData + offset));
                        float3 pos1 = mul(float4(line.endPos.xyz, 1), line.world).xyz;
                        memcpy(v1->pos, &pos1, sizeof(float) * 3);
                        v1->color = line.endColor;
                        offset += sizeof(DebugDrawVertex);

                        lineCount++;
                    }
                }

                //for (uint i = 0; i < m_wireframeBoxes.size(); ++i)
                //{
                //    VG_ASSERT(offset + 12 * sizeof(DebugDrawVertex) < m_debugDrawVBSize);
                //    
                //    if (offset + 12 * sizeof(DebugDrawVertex) < m_debugDrawVBSize)
                //    {
                //        const auto & box = m_wireframeBoxes[i];
                //    
                //        DebugDrawVertex * v0 = ((DebugDrawVertex *)(dbgDrawData + offset));
                //        float3 pos0 = float3(box.minPos.x, box.minPos.y, box.minPos.z);
                //        memcpy(v0->pos, &pos0, sizeof(float) * 3);
                //        v0->color = box.color;
                //        offset += sizeof(DebugDrawVertex);
                //    
                //        DebugDrawVertex * v1 = ((DebugDrawVertex *)(dbgDrawData + offset));
                //        float3 pos1 = float3(box.maxPos.x, box.minPos.y, box.minPos.z);
                //        memcpy(v1->pos, &pos1, sizeof(float) * 3);
                //        v1->color = box.color;
                //        offset += sizeof(DebugDrawVertex);
                //    
                //        DebugDrawVertex * v2 = ((DebugDrawVertex *)(dbgDrawData + offset));
                //        float3 pos2 = float3(box.minPos.x, box.minPos.y, box.minPos.z);
                //        memcpy(v1->pos, &pos1, sizeof(float) * 3);
                //        v1->color = box.color;
                //        offset += sizeof(DebugDrawVertex);
                //    }
                //}
            }
            _cmdList->unmap(drawData.m_debugDrawVB);
        }

        drawData.m_linesVBOffset = (u32)lineStartOffset;
        drawData.m_linesToDraw = lineCount;
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::render(const View * _view, gfx::CommandList * _cmdList)
    {
        VG_PROFILE_GPU("DebugDraw");

        const DrawData & drawData = getDrawData(_view);

        // draw lines
        if (drawData.m_linesToDraw > 0)
        {
            VG_PROFILE_GPU("Lines");

            RasterizerState rs(FillMode::Wireframe, CullMode::None);

            VG_ASSERT(nullptr != drawData.m_debugDrawVB);

            // Root sig
            _cmdList->setGraphicRootSignature(m_debugDrawSignatureHandle);

            // Root constants
            DebugDrawRootConstants3D debugDrawRoot3D;
            debugDrawRoot3D.setWorldMatrix(float4x4::identity());
            debugDrawRoot3D.setBufferHandle(drawData.m_debugDrawVB->getBufferHandle(), drawData.m_linesVBOffset);
            debugDrawRoot3D.setVertexFormat(VertexFormat::DebugDraw);
            debugDrawRoot3D.setColor(float4(1, 1, 1, 1));

            _cmdList->setRasterizerState(rs);
            _cmdList->setShader(m_debugDrawShaderKey);
            _cmdList->setPrimitiveTopology(PrimitiveTopology::LineList);

            // transparent 
            {
                debugDrawRoot3D.color = opaqueColor;
                _cmdList->setGraphicRootConstants(0, (u32 *)&debugDrawRoot3D, DebugDrawRootConstants3DCount);

                BlendState bsAlpha(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha, BlendOp::Add);
                _cmdList->setBlendState(bsAlpha);

                DepthStencilState dsAlpha(false, false, ComparisonFunc::Always);
                _cmdList->setDepthStencilState(dsAlpha);

                _cmdList->draw(drawData.m_linesToDraw << 1);
            }

            // opaque
            {
                debugDrawRoot3D.color = transparentColor;
                _cmdList->setGraphicRootConstants(0, (u32 *)&debugDrawRoot3D, DebugDrawRootConstants3DCount);

                BlendState bsOpaque(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);
                _cmdList->setBlendState(bsOpaque);

                DepthStencilState dsOpaque(true, true, ComparisonFunc::LessEqual);
                _cmdList->setDepthStencilState(dsOpaque);

                _cmdList->draw(drawData.m_linesToDraw << 1);
            }
        }

        auto world = ((IView *)_view)->GetWorld();
        if (!world)
            return;

        auto * worldData = getWorldData(world);

        // draw spheres
        {
            VG_PROFILE_CPU("Spheres");
            drawDebugModelInstances(_cmdList, m_icoSphere, worldData->m_icoSpheres);
        }

        // draw hemispheres
        {
            VG_PROFILE_CPU("HemiSpheres");
            drawDebugModelInstances(_cmdList, m_hemiSphere, worldData->m_hemiSpheres);
        }

        // draw cylinders
        {
            VG_PROFILE_CPU("Cylinders");
            drawDebugModelInstances(_cmdList, m_cylinder, worldData->m_cylinders);
        }
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::drawDebugModelInstances(gfx::CommandList * _cmdList, const MeshGeometry * _geometry, const core::vector<DebugDrawInstanceData> & _instances)
    {
        VG_PROFILE_GPU("DebugDrawSpheres");

        // Root sig
        _cmdList->setGraphicRootSignature(m_debugDrawSignatureHandle);

        RasterizerState rs(FillMode::Wireframe, CullMode::None);
        _cmdList->setRasterizerState(rs);
        _cmdList->setShader(m_debugDrawShaderKey);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleList);
        _cmdList->setIndexBuffer(_geometry->getIndexBuffer());

        const uint indexCount = _geometry->getIndexBuffer()->getBufDesc().getElementCount();

        // Root constants
        DebugDrawRootConstants3D debugDraw3D;
        debugDraw3D.setBufferHandle(_geometry->getVertexBuffer()->getBufferHandle());
        debugDraw3D.setVertexFormat(VertexFormat::DebugDraw);

        // Transparent
        {
            BlendState bsAlpha(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha, BlendOp::Add);
            _cmdList->setBlendState(bsAlpha);
            DepthStencilState dsAlpha(false, false, ComparisonFunc::Always);
            _cmdList->setDepthStencilState(dsAlpha);

            for (uint i = 0; i < _instances.size(); ++i)
            {
                const DebugDrawIcoSphereData & sphere = _instances[i];
                float4 color = unpackRGBA8(sphere.color) * transparentColor;

                debugDraw3D.setWorldMatrix(transpose(sphere.world));
                debugDraw3D.setColor(color);
                _cmdList->setGraphicRootConstants(0, (u32 *)&debugDraw3D, DebugDrawRootConstants3DCount);
                _cmdList->drawIndexed(indexCount);
            }
        }

        // Opaque
        {
            BlendState bsOpaque(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);
            _cmdList->setBlendState(bsOpaque);

            DepthStencilState dsOpaque(true, true, ComparisonFunc::LessEqual);
            _cmdList->setDepthStencilState(dsOpaque);

            for (uint i = 0; i < _instances.size(); ++i)
            {
                const DebugDrawIcoSphereData & sphere = _instances[i];
                float4 color = unpackRGBA8(sphere.color) * opaqueColor;

                debugDraw3D.setWorldMatrix(transpose(sphere.world));
                debugDraw3D.setColor(color);
                _cmdList->setGraphicRootConstants(0, (u32 *)&debugDraw3D, DebugDrawRootConstants3DCount);
                _cmdList->drawIndexed(indexCount);
            }
        }
    }
}