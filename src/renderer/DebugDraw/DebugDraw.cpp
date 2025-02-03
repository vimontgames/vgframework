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
        m_lines(1024 * 64)
    {
    }

    //--------------------------------------------------------------------------------------
    DebugDraw::WorldData::~WorldData()
    {

    }

    //--------------------------------------------------------------------------------------
    core::IDebugDrawData * DebugDraw::CreateDebugDrawData()
    {
        return new DebugDraw::WorldData();
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
        m_debugDrawOutlineShaderKey = ShaderKey("debugdraw/debugdraw.hlsl", "DebugDrawOutline");

        createBoxPrimitive();
        createCubePrimitive();
        createSquarePyramidPrimitive();
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

        VG_SAFE_RELEASE(m_box);
        VG_SAFE_RELEASE(m_gridVB);
        VG_SAFE_RELEASE(m_axisVB);
        VG_SAFE_RELEASE(m_cube);
        VG_SAFE_RELEASE(m_squarePyramid);
        VG_SAFE_RELEASE(m_icoSphere);
        VG_SAFE_RELEASE(m_hemiSphere);
        VG_SAFE_RELEASE(m_cylinder);
    }

    //--------------------------------------------------------------------------------------
    const DebugDraw::WorldData * DebugDraw::getWorldData(const core::IWorld * _world) const
    {
        return (DebugDraw::WorldData *)_world->GetDebugDrawData();
    }

    //--------------------------------------------------------------------------------------
    DebugDraw::WorldData * DebugDraw::getWorldData(const core::IWorld * _world)
    {
        VG_ASSERT(nullptr != _world);
        if (nullptr != _world)
            return (DebugDraw::WorldData *)_world->GetDebugDrawData();
        else
            return nullptr;
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

    using VertexList = std::vector<DebugDrawUnlitVertex>;
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
                _vertices.push_back(DebugDrawUnlitVertex(point.x, point.y, point.z));
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
            BufferDesc vbDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(DebugDrawUnlitVertex), (uint)vertices.size());
            Buffer * vb = device->createBuffer(vbDesc, "IcoSphereVB", vertices.data());

            const uint indiceCount = (uint)triangles.size() * 3;
            BufferDesc ibDesc(Usage::Default, BindFlags::IndexBuffer | BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(u16), indiceCount);
            Buffer * ib = device->createBuffer(ibDesc, "IcoSphereIB", triangles.data());

            m_icoSphere = new MeshGeometry("IcoSphere", this);
            m_icoSphere->setVertexFormat(VertexFormat::DebugDrawUnlit);
            m_icoSphere->setIndexBuffer(ib);
            m_icoSphere->setVertexBuffer(vb);
            m_icoSphere->addBatch("IcoBatch", indiceCount);

            VG_SAFE_RELEASE(ib);
            VG_SAFE_RELEASE(vb);
        }

        // Create Hemisphere by discarding triangles z < 0 and clamping
        {
            VertexList hemiVertices;
            TriangleList hemiTriangles;

            auto pushVertex = [](VertexList & hemiVertices, DebugDrawUnlitVertex & _vertex)
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
                    if (!memcmp(&hemiVertices[j], &_vertex, sizeof(DebugDrawUnlitVertex)))
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

            BufferDesc vbDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(DebugDrawUnlitVertex), (uint)hemiVertices.size());
            Buffer * vb = device->createBuffer(vbDesc, "HemiSphereVB", hemiVertices.data());

            const uint indiceCount = (uint)hemiTriangles.size() * 3;
            BufferDesc ibDesc(Usage::Default, BindFlags::IndexBuffer | BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(u16), indiceCount);
            Buffer * ib = device->createBuffer(ibDesc, "HemiSphereIB", hemiTriangles.data());

            m_hemiSphere = new MeshGeometry("HemiSphere", this);
            m_hemiSphere->setVertexFormat(VertexFormat::DebugDrawUnlit);
            m_hemiSphere->setIndexBuffer(ib);
            m_hemiSphere->setVertexBuffer(vb);
            m_hemiSphere->addBatch("HemiBatch", indiceCount);

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

        auto pushVertex = [](VertexList & vertices, DebugDrawUnlitVertex & _vertex)
        {
            const uint count = (uint)vertices.size();
            for (uint j = 0; j < count; ++j)
            {
                if (!memcmp(&vertices[j], &_vertex, sizeof(DebugDrawUnlitVertex)))
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

            auto bottom = DebugDrawUnlitVertex(x, y, -height);
            auto top = DebugDrawUnlitVertex(x, y, height);

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
                triangles.push_back({ vbot, prevBot, vtop });
            }

            prevBot = vbot;
            prevTop = vtop;
        }

        triangles.push_back({ prevTop, firstTop, firstBot });
        triangles.push_back({ firstBot, prevBot, prevTop });

        auto * device = Device::get();

        BufferDesc vbDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(DebugDrawUnlitVertex), (uint)vertices.size());
        Buffer * vb = device->createBuffer(vbDesc, "CylinderVB", vertices.data());

        const uint indiceCount = (uint)triangles.size() * 3;
        BufferDesc ibDesc(Usage::Default, BindFlags::IndexBuffer | BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(u16), indiceCount);
        Buffer * ib = device->createBuffer(ibDesc, "CylinderIB", triangles.data());

        m_cylinder = new MeshGeometry("Cylinder", this);
        m_cylinder->setVertexFormat(VertexFormat::DebugDrawUnlit);
        m_cylinder->setIndexBuffer(ib);
        m_cylinder->setVertexBuffer(vb);
        m_cylinder->addBatch("CylinderBatch", indiceCount);

        VG_SAFE_RELEASE(ib);
        VG_SAFE_RELEASE(vb);
    }

    //--------------------------------------------------------------------------------------
    // Box primitive is used to render wireframe bounding box using lines (without the triangles inside face)
    //--------------------------------------------------------------------------------------
    void DebugDraw::createBoxPrimitive()
    {
        auto * device = Device::get();

        DebugDrawUnlitVertex vertices[8];

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

        BufferDesc vbDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(DebugDrawUnlitVertex), (uint)countof(vertices));
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
        BufferDesc ibDesc(Usage::Default, BindFlags::IndexBuffer | BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(u16), indiceCount);
        Buffer * ib = device->createBuffer(ibDesc, "BoxIB", indices);

        m_box = new MeshGeometry("Box", this);
        m_box->setVertexFormat(VertexFormat::DebugDrawUnlit);
        m_box->setIndexBuffer(ib);
        m_box->setVertexBuffer(vb);
        m_box->addBatch("BoxBatch", indiceCount);

        VG_SAFE_RELEASE(ib);
        VG_SAFE_RELEASE(vb);
    }

    //--------------------------------------------------------------------------------------
    // Cube primitive is used to draw solid cube using index triangles geometry
    //--------------------------------------------------------------------------------------
    void DebugDraw::createCubePrimitive()
    {
        auto * device = Device::get();

        DebugDrawLitVertex vertices[24];

        // Front face
        vertices[0].setPos({ -1.0f, -1.0f, -1.0f });
        vertices[0].setColor(0xFFFFFFFF);
        vertices[0].setNormal({ 0.0f, 0.0f, -1.0f });

        vertices[1].setPos({ +1.0f, -1.0f, -1.0f });
        vertices[1].setColor(0xFFFFFFFF);
        vertices[1].setNormal({ 0.0f, 0.0f, -1.0f });

        vertices[2].setPos({ +1.0f, +1.0f, -1.0f });
        vertices[2].setColor(0xFFFFFFFF);
        vertices[2].setNormal({ 0.0f, 0.0f, -1.0f });

        vertices[3].setPos({ -1.0f, +1.0f, -1.0f });
        vertices[3].setColor(0xFFFFFFFF);
        vertices[3].setNormal({ 0.0f, 0.0f, -1.0f });

        // Back face
        vertices[4].setPos({ -1.0f, -1.0f, +1.0f });
        vertices[4].setColor(0xFFFFFFFF);
        vertices[4].setNormal({ 0.0f, 0.0f, 1.0f });

        vertices[5].setPos({ +1.0f, -1.0f, +1.0f });
        vertices[5].setColor(0xFFFFFFFF);
        vertices[5].setNormal({ 0.0f, 0.0f, 1.0f });

        vertices[6].setPos({ +1.0f, +1.0f, +1.0f });
        vertices[6].setColor(0xFFFFFFFF);
        vertices[6].setNormal({ 0.0f, 0.0f, 1.0f });

        vertices[7].setPos({ -1.0f, +1.0f, +1.0f });
        vertices[7].setColor(0xFFFFFFFF);
        vertices[7].setNormal({ 0.0f, 0.0f, 1.0f });

        // Left face
        vertices[8].setPos({ -1.0f, -1.0f, -1.0f });
        vertices[8].setColor(0xFFFFFFFF);
        vertices[8].setNormal({ -1.0f, 0.0f, 0.0f });

        vertices[9].setPos({ -1.0f, +1.0f, -1.0f });
        vertices[9].setColor(0xFFFFFFFF);
        vertices[9].setNormal({ -1.0f, 0.0f, 0.0f });

        vertices[10].setPos({ -1.0f, +1.0f, +1.0f });
        vertices[10].setColor(0xFFFFFFFF);
        vertices[10].setNormal({ -1.0f, 0.0f, 0.0f });

        vertices[11].setPos({ -1.0f, -1.0f, +1.0f });
        vertices[11].setColor(0xFFFFFFFF);
        vertices[11].setNormal({ -1.0f, 0.0f, 0.0f });

        // Right face
        vertices[12].setPos({ +1.0f, -1.0f, -1.0f });
        vertices[12].setColor(0xFFFFFFFF);
        vertices[12].setNormal({ 1.0f, 0.0f, 0.0f });

        vertices[13].setPos({ +1.0f, +1.0f, -1.0f });
        vertices[13].setColor(0xFFFFFFFF);
        vertices[13].setNormal({ 1.0f, 0.0f, 0.0f });

        vertices[14].setPos({ +1.0f, +1.0f, +1.0f });
        vertices[14].setColor(0xFFFFFFFF);
        vertices[14].setNormal({ 1.0f, 0.0f, 0.0f });

        vertices[15].setPos({ +1.0f, -1.0f, +1.0f });
        vertices[15].setColor(0xFFFFFFFF);
        vertices[15].setNormal({ 1.0f, 0.0f, 0.0f });

        // Top face
        vertices[16].setPos({ -1.0f, +1.0f, -1.0f });
        vertices[16].setColor(0xFFFFFFFF);
        vertices[16].setNormal({ 0.0f, 1.0f, 0.0f });

        vertices[17].setPos({ +1.0f, +1.0f, -1.0f });
        vertices[17].setColor(0xFFFFFFFF);
        vertices[17].setNormal({ 0.0f, 1.0f, 0.0f });

        vertices[18].setPos({ +1.0f, +1.0f, +1.0f });
        vertices[18].setColor(0xFFFFFFFF);
        vertices[18].setNormal({ 0.0f, 1.0f, 0.0f });

        vertices[19].setPos({ -1.0f, +1.0f, +1.0f });
        vertices[19].setColor(0xFFFFFFFF);
        vertices[19].setNormal({ 0.0f, 1.0f, 0.0f });

        // Bottom face
        vertices[20].setPos({ -1.0f, -1.0f, -1.0f });
        vertices[20].setColor(0xFFFFFFFF);
        vertices[20].setNormal({ 0.0f, -1.0f, 0.0f });

        vertices[21].setPos({ +1.0f, -1.0f, -1.0f });
        vertices[21].setColor(0xFFFFFFFF);
        vertices[21].setNormal({ 0.0f, -1.0f, 0.0f });

        vertices[22].setPos({ +1.0f, -1.0f, +1.0f });
        vertices[22].setColor(0xFFFFFFFF);
        vertices[22].setNormal({ 0.0f, -1.0f, 0.0f });

        vertices[23].setPos({ -1.0f, -1.0f, +1.0f });
        vertices[23].setColor(0xFFFFFFFF);
        vertices[23].setNormal({ 0.0f, -1.0f, 0.0f });

        // Additional faces added
        BufferDesc vbDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(DebugDrawLitVertex), (uint)countof(vertices));
        Buffer * vb = device->createBuffer(vbDesc, "CubeVB", vertices);

        const u16 indices[] =
        {
            1, 2, 0,  2, 3, 0,      // Front
            4, 6, 5,  6, 4, 7,      // Back
            8, 9, 10,  10, 11, 8,    // Left 
            12, 14, 13,  14, 12, 15, // Right
            16, 17, 18,  18, 19, 16, // Top 
            20, 22, 21,  22, 20, 23  // Bottom
        };

        const uint indiceCount = (uint)countof(indices);
        BufferDesc ibDesc(Usage::Default, BindFlags::IndexBuffer | BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(u16), indiceCount);
        Buffer * ib = device->createBuffer(ibDesc, "CubeIB", indices);

        m_cube = new MeshGeometry("Cube", this);
        m_cube->setVertexFormat(VertexFormat::DebugDrawLit);
        m_cube->setIndexBuffer(ib);
        m_cube->setVertexBuffer(vb);
        m_cube->addBatch("CubeBatch", indiceCount);

        VG_SAFE_RELEASE(ib);
        VG_SAFE_RELEASE(vb);
    }

    
    //--------------------------------------------------------------------------------------
    void DebugDraw::createSquarePyramidPrimitive()
    {
        auto * device = Device::get();

        // Vertex data for the square pyramid, each face gets its own set of vertices
        DebugDrawLitVertex vertices[18];

        // Base face (4 vertices, with the same normal pointing downwards)
        vertices[0].setPos({ -1.0f, -1.0f, 0.0f });
        vertices[0].setNormal({ 0.0f, 0.0f, -1.0f });
        vertices[0].setColor(0xFFFFFFFF);

        vertices[1].setPos({ +1.0f, -1.0f, 0.0f });
        vertices[1].setNormal({ 0.0f, 0.0f, -1.0f });
        vertices[1].setColor(0xFFFFFFFF);

        vertices[2].setPos({ +1.0f, +1.0f, 0.0f });
        vertices[2].setNormal({ 0.0f, 0.0f, -1.0f });
        vertices[2].setColor(0xFFFFFFFF);

        vertices[3].setPos({ -1.0f, +1.0f, 0.0f });
        vertices[3].setNormal({ 0.0f, 0.0f, -1.0f });
        vertices[3].setColor(0xFFFFFFFF);

        // Side faces (each face gets 3 new vertices with the correct normal)
        float3 apex = { 0.0f, 0.0f, +1.5f };

        // Front left face (normal is pointing outwards)
        vertices[4].setPos({ -1.0f, -1.0f, 0.0f });
        vertices[4].setNormal({ -0.577f, -0.577f, +0.577f }); // Normal
        vertices[4].setColor(0xFFFFFFFF);

        vertices[5].setPos({ +1.0f, -1.0f, 0.0f });
        vertices[5].setNormal({ -0.577f, -0.577f, +0.577f }); // Normal
        vertices[5].setColor(0xFFFFFFFF);

        vertices[6].setPos(apex);
        vertices[6].setNormal({ -0.577f, -0.577f, +0.577f });
        vertices[6].setColor(0xFFFFFFFF);

        // Front right face
        vertices[7].setPos({ +1.0f, -1.0f, 0.0f });
        vertices[7].setNormal({ +0.577f, -0.577f, +0.577f });
        vertices[7].setColor(0xFFFFFFFF);

        vertices[8].setPos({ +1.0f, +1.0f, 0.0f });
        vertices[8].setNormal({ +0.577f, -0.577f, +0.577f });
        vertices[8].setColor(0xFFFFFFFF);

        vertices[9].setPos(apex);
        vertices[9].setNormal({ +0.577f, -0.577f, +0.577f });
        vertices[9].setColor(0xFFFFFFFF);

        // Back right face
        vertices[10].setPos({ +1.0f, +1.0f, 0.0f });
        vertices[10].setNormal({ +0.577f, +0.577f, +0.577f });
        vertices[10].setColor(0xFFFFFFFF);

        vertices[11].setPos({ -1.0f, +1.0f, 0.0f });
        vertices[11].setNormal({ +0.577f, +0.577f, +0.577f });
        vertices[11].setColor(0xFFFFFFFF);

        vertices[12].setPos(apex);
        vertices[12].setNormal({ +0.577f, +0.577f, +0.577f });
        vertices[12].setColor(0xFFFFFFFF);

        // Back left face
        vertices[13].setPos({ -1.0f, +1.0f, 0.0f });
        vertices[13].setNormal({ -0.577f, +0.577f, +0.577f });
        vertices[13].setColor(0xFFFFFFFF);

        vertices[14].setPos({ -1.0f, -1.0f, 0.0f });
        vertices[14].setNormal({ -0.577f, +0.577f, +0.577f });
        vertices[14].setColor(0xFFFFFFFF);

        vertices[15].setPos(apex);
        vertices[15].setNormal({ -0.577f, +0.577f, +0.577f });
        vertices[15].setColor(0xFFFFFFFF);

        // Index buffer (updated for the new vertex structure)
        const u16 indices[] =
        {
            0, 1, 2,  2, 3, 0,  // Base face 
            4, 6, 5,            // Front left face
            7, 9, 8,            // Front right face
            10, 12, 11,         // Back right face
            13, 15, 14          // Back left face
        };

        const uint indiceCount = (uint)countof(indices);

        // Vertex buffer
        BufferDesc vbDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(DebugDrawLitVertex), (uint)countof(vertices));
        Buffer * vb = device->createBuffer(vbDesc, "PyramidVB", vertices);

        // Index buffer
        BufferDesc ibDesc(Usage::Default, BindFlags::IndexBuffer | BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(u16), indiceCount);
        Buffer * ib = device->createBuffer(ibDesc, "PyramidIB", indices);

        // Create the mesh
        m_squarePyramid = new MeshGeometry("Pyramid", this);
        m_squarePyramid->setVertexFormat(VertexFormat::DebugDrawLit);  
        m_squarePyramid->setVertexBuffer(vb);
        m_squarePyramid->setIndexBuffer(ib);
        m_squarePyramid->addBatch("PyramidBatch", indiceCount);

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

        vector<DebugDrawUnlitVertex> vertices;
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

        BufferDesc vbDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(DebugDrawUnlitVertex), (u32)vertices.size());
        m_gridVB = device->createBuffer(vbDesc, "GridVB", vertices.data());
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::createAxis()
    {
        auto * device = Device::get();

        DebugDrawUnlitVertex vertices[6];

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

        BufferDesc vbDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, BufferFlags::None, sizeof(DebugDrawUnlitVertex), (uint)countof(vertices));

        m_axisVB = device->createBuffer(vbDesc, "AxisVB", vertices);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::drawAABB(CommandList * _cmdList, const core::AABB & _aabb, const float4x4 & _world) const
    {
        RasterizerState rs(FillMode::Wireframe, CullMode::None);
        _cmdList->setRasterizerState(rs);

        // AABB Matrix in world-space
        float4x4 aabbMatrixWS = mul(_aabb.getLocalSpaceUnitCubeMatrix(), _world);

        DebugDrawRootConstants3D debugDraw3D;
        {
            debugDraw3D.setWorldMatrix(transpose(aabbMatrixWS));
            debugDraw3D.setVertexBufferHandle(m_box->getVertexBuffer()->getBufferHandle(), m_box->getVertexBufferOffset());
            debugDraw3D.setVertexFormat(VertexFormat::DebugDrawUnlit);
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
        DepthStencilState ds(true, false, ComparisonFunc::LessEqual);
        BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);

        VG_ASSERT(nullptr != m_gridVB);
        const BufferDesc & gridDesc = m_gridVB->getBufDesc();

        DebugDrawRootConstants3D debugDrawRoot3D;

        debugDrawRoot3D.setWorldMatrix(float4x4::identity());
        debugDrawRoot3D.setVertexBufferHandle(m_gridVB->getBufferHandle());
        debugDrawRoot3D.setVertexFormat(VertexFormat::DebugDrawUnlit);
        debugDrawRoot3D.setColor(float4(1, 1, 1, 1));

        _cmdList->setRasterizerState(rs);
        _cmdList->setDepthStencilState(ds);
        _cmdList->setBlendState(bs);
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
        DepthStencilState ds(true, false, ComparisonFunc::LessEqual);
        BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);

        VG_ASSERT(nullptr != m_axisVB);
        const BufferDesc & gridDesc = m_axisVB->getBufDesc();

        DebugDrawRootConstants3D debugDrawRoot3D;

        debugDrawRoot3D.setWorldMatrix(float4x4::identity());
        debugDrawRoot3D.setVertexBufferHandle(m_axisVB->getBufferHandle());
        debugDrawRoot3D.setVertexFormat(VertexFormat::DebugDrawUnlit);
        debugDrawRoot3D.setColor(float4(1, 1, 1, 1));

        _cmdList->setRasterizerState(rs);
        _cmdList->setDepthStencilState(ds);
        _cmdList->setBlendState(bs);
        _cmdList->setShader(m_debugDrawShaderKey);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::LineList);

        _cmdList->setGraphicRootConstants(0, (u32 *)&debugDrawRoot3D, DebugDrawRootConstants3DCount);
        _cmdList->draw(6);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::AddLine(const core::IWorld * _world, const core::float3 & _beginPos, const core::float3 & _endPos, core::u32 _color, const core::float4x4 & _matrix, PickingID _pickingID)
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
    void DebugDraw::AddWireframeCube(const core::IWorld * _world, const float3 & _minPos, const float3 & _maxPos, u32 _color, const float4x4 & _matrix, PickingID _pickingID)
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
    void DebugDraw::AddSolidCube(const core::IWorld * _world, const core::float3 & _minPos, const core::float3 & _maxPos, core::u32 _color, const core::float4x4 & _matrix, PickingID _pickingID)
    {
        DebugDrawCubeData & cube = getWorldData(_world)->m_cubes[asInteger(DebugDrawFillMode::Solid)].emplace_back();
        cube.world = getCubeMatrix(_matrix, _minPos, _maxPos);
        cube.pickingID = _pickingID;
        cube.color = _color;
    }

    //--------------------------------------------------------------------------------------
    float4x4 DebugDraw::getCubeMatrix(const core::float4x4 & _matrix, const core::float3 & _minPos, const core::float3 & _maxPos) const
    {
        return mul(float4x4::scale(_maxPos - _minPos), _matrix);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::AddWireframeSphere(const core::IWorld * _world, const float _radius, core::u32 _color, const core::float4x4 & _matrix, PickingID _pickingID)
    {
        addSphere(_world, _radius, _color, _matrix, _pickingID, DebugDrawFillMode::Wireframe);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::AddSolidSphere(const core::IWorld * _world, float _radius, core::u32 _color, const core::float4x4 & _matrix, PickingID _pickingID)
    {
        addSphere(_world, _radius, _color, _matrix, _pickingID, DebugDrawFillMode::Solid);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::addSphere(const core::IWorld * _world, float _radius, core::u32 _color, const core::float4x4 & _matrix, PickingID _pickingID, DebugDrawFillMode _fillmode)
    {
        DebugDrawIcoSphereData & icoSphere = getWorldData(_world)->m_icoSpheres[asInteger(_fillmode)].emplace_back();
        icoSphere.world = getIcoSphereMatrix(_matrix, _radius);
        icoSphere.pickingID = _pickingID;
        icoSphere.color = _color;
    }

    //--------------------------------------------------------------------------------------
    core::float4x4 DebugDraw::getIcoSphereMatrix(const core::float4x4 & _matrix, float _radius) const
    {
        return mul(float4x4::scale(_radius), _matrix);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::AddWireframeHemisphere(const core::IWorld * _world, const float _radius, core::u32 _color, const core::float4x4 & _matrix, PickingID _pickingID)
    {
        addHemisphere(_world, _radius, _color, _matrix, _pickingID, DebugDrawFillMode::Wireframe);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::AddSolidHemisphere(const core::IWorld * _world, float _radius, core::u32 _color, const core::float4x4 & _matrix, PickingID _pickingID)
    {
        addHemisphere(_world, _radius, _color, _matrix, _pickingID, DebugDrawFillMode::Solid);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::addHemisphere(const core::IWorld * _world, const float _radius, core::u32 _color, const core::float4x4 & _matrix, PickingID _pickingID, DebugDrawFillMode _fillmode)
    {
        DebugDrawHemiSphereData & hemiSphere = getWorldData(_world)->m_hemiSpheres[asInteger(_fillmode)].emplace_back();
        hemiSphere.world = mul(float4x4::scale(_radius), _matrix);
        hemiSphere.pickingID = _pickingID;
        hemiSphere.color = _color;
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::AddWireframeCylinder(const core::IWorld * _world, float _radius, float _height, core::u32 _color, const core::float4x4 & _matrix, PickingID _pickingID)
    {
        addCylinder(_world, _radius, _height, _color, _matrix, _pickingID, DebugDrawFillMode::Wireframe);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::AddSolidCylinder(const core::IWorld * _world, float _radius, float _height, core::u32 _color, const core::float4x4 & _matrix, PickingID _pickingID)
    {
        addCylinder(_world, _radius, _height, _color, _matrix, _pickingID, DebugDrawFillMode::Solid);
    }

    //--------------------------------------------------------------------------------------
    float4x4 DebugDraw::getCylinderMatrix(const core::float4x4 & _matrix, float _radius, float _height) const
    {
        float3 s = float3(_radius, _radius, _height * 0.5f);
        const float4x4 scale = float4x4
        (
            s.x, 0, 0, 0,
            0, s.y, 0, 0,
            0, 0, s.z, 0,
            0, 0, 0, 1
        );
        return mul(scale, _matrix);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::addCylinder(const core::IWorld * _world, float _radius, float _height, core::u32 _color, const core::float4x4 & _matrix, PickingID _pickingID, DebugDrawFillMode _fillmode)
    {
        DebugDrawCylinderData & cylinder = getWorldData(_world)->m_cylinders[asInteger(_fillmode)].emplace_back();

        cylinder.world = getCylinderMatrix(_matrix, _radius, _height);
        cylinder.pickingID = _pickingID;
        cylinder.color = _color;
        cylinder.taper = 1.0f;
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::AddWireframeTaperedCylinder(const core::IWorld * _world, float _topRadius, float _bottomRadius, float _height, core::u32 _color, const core::float4x4 & _matrix, PickingID _pickingID)
    {
        DebugDrawCylinderData & cylinder = getWorldData(_world)->m_cylinders[asInteger(DebugDrawFillMode::Wireframe)].emplace_back();

        float radius = _bottomRadius;
        float3 s = float3(radius, radius, _height * 0.5f);

        const float4x4 scale = float4x4
        (
            s.x,   0,   0, 0,
              0, s.y,   0, 0,
              0,   0, s.z, 0,
              0,   0,   0, 1 
        );

        cylinder.world = mul(scale, _matrix);
        cylinder.pickingID = _pickingID;
        cylinder.color = _color;
        cylinder.taper = _topRadius / _bottomRadius;
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::AddWireframeCapsule(const core::IWorld * _world, float _radius, float _height, core::u32 _color, const core::float4x4 & _matrix, PickingID _pickingID)
    {
        float offset = max(0.0f, _height - _radius * 2.0f);

        float4x4 topHemi = float4x4::identity();
        topHemi[3].xyz = float3(0, 0, 0.5f * offset);
        AddWireframeHemisphere(_world, _radius, _color, mul(topHemi, _matrix), _pickingID);

        AddWireframeCylinder(_world, _radius, offset, _color, _matrix, _pickingID);

        float4x4 bottomHemi = float4x4::identity();
        bottomHemi[2].xyz *= -1.0f;
        bottomHemi[3].xyz = float3(0, 0, -0.5f * offset);
        AddWireframeHemisphere(_world, _radius, _color, mul(bottomHemi, _matrix), _pickingID);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::AddWireframeTaperedCapsule(const core::IWorld * _world, float _topRadius, float _bottomRadius, float _height, core::u32 _color, const core::float4x4 & _matrix, PickingID _pickingID)
    {
        float offset = max(0.0f, _height - (_topRadius + _bottomRadius));

        float4x4 topHemi = float4x4::identity();
        topHemi[3].xyz = float3(0, 0, 0.5f * offset);
        AddWireframeHemisphere(_world, _topRadius, _color, mul(topHemi, _matrix), _pickingID);

        AddWireframeTaperedCylinder(_world, _topRadius, _bottomRadius, offset, _color, _matrix, _pickingID);

        float4x4 bottomHemi = float4x4::identity();
        bottomHemi[2].xyz *= -1.0f;
        bottomHemi[3].xyz = float3(0, 0, -0.5f * offset);
        AddWireframeHemisphere(_world, _bottomRadius, _color, mul(bottomHemi, _matrix), _pickingID);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::AddWireframeSquarePyramid(const core::IWorld * _world, float _base, float _height, core::u32 _color, const core::float4x4 & _matrix, PickingID _pickingID)
    {
        addSquarePyramid(_world, _base, _height, _color, _matrix, _pickingID, DebugDrawFillMode::Wireframe);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::AddSolidSquarePyramid(const core::IWorld * _world, float _base, float _height, core::u32 _color, const core::float4x4 & _matrix, PickingID _pickingID)
    {
        addSquarePyramid(_world, _base, _height, _color, _matrix, _pickingID, DebugDrawFillMode::Solid);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::addSquarePyramid(const core::IWorld * _world, float _base, float _height, core::u32 _color, const core::float4x4 & _matrix, PickingID _pickingID, DebugDrawFillMode _fillmode)
    {
        DebugDrawSquarePyramidData & squarePyramid = getWorldData(_world)->m_squarePyramid[asInteger(_fillmode)].emplace_back();

        squarePyramid.world = getSquarePyramidMatrix(_matrix, _base, _height);
        squarePyramid.pickingID = _pickingID;
        squarePyramid.color = _color;
    }

    //--------------------------------------------------------------------------------------
    float4x4 DebugDraw::getSquarePyramidMatrix(const core::float4x4 & _matrix, float _base, float _height) const
    {
        float3 s = float3(_base, _base, _height);

        const float4x4 scale = float4x4
        (
            s.x, 0, 0, 0,
            0, s.y, 0, 0,
            0, 0, s.z, 0,
            0, 0, 0, 1
        );

        return mul(scale, _matrix);
    }

    //--------------------------------------------------------------------------------------
    const vg::engine::IEngine * getEngine()
    {
        const auto * factory = Kernel::getFactory();
        return (const vg::engine::IEngine *)factory->GetSingleton("Engine");
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

                for (uint i = 0; i < countof(worldData->m_squarePyramid); ++i)
                    worldData->m_squarePyramid[i].clear();

                for (uint i = 0; i < countof(worldData->m_cubes); ++i)
                    worldData->m_cubes[i].clear();

                for (uint i = 0; i < countof(worldData->m_hemiSpheres); ++i)
                    worldData->m_hemiSpheres[i].clear();

                for (uint i = 0; i < countof(worldData->m_cylinders); ++i)
                    worldData->m_cylinders[i].clear();

                for (uint i = 0; i < countof(worldData->m_icoSpheres); ++i)
                    worldData->m_icoSpheres[i].clear();
            }
        }
    }

    //--------------------------------------------------------------------------------------
    u64 DebugDraw::getDebugDrawCount(const View * _view) const
    {
        auto world = ((IView *)_view)->GetWorld();
        if (world)
        {
            const auto * worldData = getWorldData(world);

            auto count = worldData->m_lines.size();

            for (uint i = 0; i < countof(worldData->m_hemiSpheres); ++i)
                count += worldData->m_cylinders[i].size();

            for (uint i = 0; i < countof(worldData->m_cylinders); ++i)
                count += worldData->m_hemiSpheres[i].size();

            for (uint i = 0; i < countof(worldData->m_icoSpheres); ++i)
                count += worldData->m_icoSpheres[i].size();

            return count;
        }
        return 0;
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::reset()
    {
        clearDrawData();
    }

    const float4 opaqueColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    const float4 transparentColor = float4(0.75f, 0.75f, 0.75f, 0.5f);

    //--------------------------------------------------------------------------------------
    void DebugDraw::update(const View * _view, gfx::CommandList * _cmdList)
    {
        VG_PROFILE_CPU("DebugDrawUpdate");

        // Wait DebugDraw jobs
        {
            VG_PROFILE_CPU("Wait DebugDraw");
            Renderer::get()->WaitJobSync(RendererJobType::DebugDraw);
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

        const auto mapSizeInBytes = worldData->m_lines.size() * 2 * sizeof(DebugDrawUnlitVertex);
        if (mapSizeInBytes > 0)
        {
            u8 * dbgDrawData = (u8 *)_cmdList->map(drawData.m_debugDrawVB, mapSizeInBytes).data;
            {
                for (uint i = 0; i < worldData->m_lines.size(); ++i)
                {
                    VG_ASSERT(offset + 2 * sizeof(DebugDrawUnlitVertex) < drawData.m_debugDrawVBSize);

                    if (uint_ptr(offset + 2 * sizeof(DebugDrawUnlitVertex)) < drawData.m_debugDrawVBSize)
                    {
                        const auto & line = worldData->m_lines[i];

                        DebugDrawUnlitVertex * v0 = ((DebugDrawUnlitVertex *)(dbgDrawData + offset));
                        float3 pos0 = mul(float4(line.beginPos.xyz, 1), line.world).xyz;
                        memcpy(v0->pos, &pos0, sizeof(float) * 3);
                        v0->color = line.beginColor;
                        offset += sizeof(DebugDrawUnlitVertex);

                        DebugDrawUnlitVertex * v1 = ((DebugDrawUnlitVertex *)(dbgDrawData + offset));
                        float3 pos1 = mul(float4(line.endPos.xyz, 1), line.world).xyz;
                        memcpy(v1->pos, &pos1, sizeof(float) * 3);
                        v1->color = line.endColor;
                        offset += sizeof(DebugDrawUnlitVertex);

                        lineCount++;
                    }
                }
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

            RasterizerState rs(FillMode::Wireframe, CullMode::None, Orientation::ClockWise, DepthClip::Disable);

            VG_ASSERT(nullptr != drawData.m_debugDrawVB);

            // Root sig
            _cmdList->setGraphicRootSignature(m_debugDrawSignatureHandle);

            // Root constants
            DebugDrawRootConstants3D debugDrawRoot3D;
            debugDrawRoot3D.setWorldMatrix(float4x4::identity());
            debugDrawRoot3D.setVertexBufferHandle(drawData.m_debugDrawVB->getBufferHandle(), drawData.m_linesVBOffset);
            debugDrawRoot3D.setVertexFormat(VertexFormat::DebugDrawUnlit);
            debugDrawRoot3D.setColor(float4(1, 1, 1, 1));

            _cmdList->setRasterizerState(rs);
            _cmdList->setShader(m_debugDrawShaderKey);
            _cmdList->setPrimitiveTopology(PrimitiveTopology::LineList);

            // transparent 
            {
                debugDrawRoot3D.color = transparentColor;
                _cmdList->setGraphicRootConstants(0, (u32 *)&debugDrawRoot3D, DebugDrawRootConstants3DCount);

                BlendState bsAlpha(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha, BlendOp::Add);
                _cmdList->setBlendState(bsAlpha);

                DepthStencilState dsAlpha(false, false, ComparisonFunc::Always);
                _cmdList->setDepthStencilState(dsAlpha);

                _cmdList->draw(drawData.m_linesToDraw << 1);
            }

            // opaque
            {
                debugDrawRoot3D.color = opaqueColor;
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

        // draw solid primitives
        {
            VG_PROFILE_CPU("Solid");
            VG_PROFILE_GPU("Solid");
            {
                VG_PROFILE_CPU("Cube");
                VG_PROFILE_GPU("Cube");
                drawDebugModelInstances(_cmdList, m_cube, worldData->m_cubes, DebugDrawFillMode::Solid);
            }
            {
                VG_PROFILE_CPU("SquarePyramid");
                VG_PROFILE_GPU("SquarePyramid");
                drawDebugModelInstances(_cmdList, m_squarePyramid, worldData->m_squarePyramid, DebugDrawFillMode::Solid);
            }
            {
                VG_PROFILE_CPU("Spheres");
                VG_PROFILE_GPU("Spheres");
                drawDebugModelInstances(_cmdList, m_icoSphere, worldData->m_icoSpheres, DebugDrawFillMode::Solid);
            }
            {
                VG_PROFILE_CPU("HemiSpheres");
                VG_PROFILE_GPU("HemiSpheres");
                drawDebugModelInstances(_cmdList, m_hemiSphere, worldData->m_hemiSpheres, DebugDrawFillMode::Solid);
            }
            {
                VG_PROFILE_CPU("Cylinders");
                VG_PROFILE_GPU("Cylinders");
                drawDebugModelInstances(_cmdList, m_cylinder, worldData->m_cylinders, DebugDrawFillMode::Solid);
            }
        }

        // draw wireframe primitives
        {
            VG_PROFILE_CPU("Wireframe");
            VG_PROFILE_GPU("Wireframe");
            {
                VG_PROFILE_CPU("Spheres");
                VG_PROFILE_GPU("Spheres");
                drawDebugModelInstances(_cmdList, m_icoSphere, worldData->m_icoSpheres, DebugDrawFillMode::Wireframe);
            }
            {
                VG_PROFILE_CPU("SquarePyramid");
                VG_PROFILE_GPU("SquarePyramid");
                drawDebugModelInstances(_cmdList, m_squarePyramid, worldData->m_squarePyramid, DebugDrawFillMode::Wireframe);
            }
            {
                VG_PROFILE_CPU("HemiSpheres");
                VG_PROFILE_GPU("HemiSpheres");
                drawDebugModelInstances(_cmdList, m_hemiSphere, worldData->m_hemiSpheres, DebugDrawFillMode::Wireframe);
            }
            {
                VG_PROFILE_CPU("Cylinders");
                VG_PROFILE_GPU("Cylinders");
                drawDebugModelInstances(_cmdList, m_cylinder, worldData->m_cylinders, DebugDrawFillMode::Wireframe);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    bool DebugDraw::setupDebugModelInstances(gfx::CommandList * _cmdList, const MeshGeometry * _geometry, DebugDrawFillMode _fillmode, DebugDrawRootConstants3D & _debugDraw3D, core::uint & _indexCount) const
    {
        VG_ASSERT(nullptr != _geometry);
        if (nullptr == _geometry)
            return false;

        // Root sig
        _cmdList->setGraphicRootSignature(m_debugDrawSignatureHandle);

        switch (_fillmode)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_fillmode);

            case DebugDrawFillMode::Wireframe:
            {
                RasterizerState rs(FillMode::Wireframe, CullMode::Back);
                _cmdList->setRasterizerState(rs);
            }
            break;

            case DebugDrawFillMode::Solid:
            case DebugDrawFillMode::Outline:
            {
                RasterizerState rs(FillMode::Solid, CullMode::Back);
                _cmdList->setRasterizerState(rs);
            }
            break;
        }

        switch (_fillmode)
        {
            case DebugDrawFillMode::Wireframe:
            case DebugDrawFillMode::Solid:
                _cmdList->setShader(m_debugDrawShaderKey);
                break;

            case DebugDrawFillMode::Outline:
                _cmdList->setShader(m_debugDrawOutlineShaderKey);
                break;
        }
        
        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleList);
        _cmdList->setIndexBuffer(_geometry->getIndexBuffer());

        _indexCount = _geometry->getIndexBuffer()->getBufDesc().getElementCount();

        // Root constants
        _debugDraw3D.setVertexBufferHandle(_geometry->getVertexBuffer()->getBufferHandle());
        _debugDraw3D.setVertexFormat(_geometry->getVertexFormat());

        return true;
    }

    //--------------------------------------------------------------------------------------
    template <typename T, size_t N> void DebugDraw::drawDebugModelInstances(gfx::CommandList * _cmdList, const MeshGeometry * _geometry, const core::vector<T> (& _instances)[N], DebugDrawFillMode _fillmode) const
    {
        DebugDrawRootConstants3D debugDraw3D;
        uint indexCount;

        if (setupDebugModelInstances(_cmdList, _geometry, _fillmode, debugDraw3D, indexCount))
        {
            auto & instances = _instances[asInteger(_fillmode)];

            // Transparent
            {
                setupZFail(_cmdList);

                for (uint i = 0; i < instances.size(); ++i)
                {
                    const T & instance = instances[i];
                    float4 color = unpackRGBA8(instance.color) * transparentColor;

                    debugDraw3D.setWorldMatrix(transpose(instance.world));
                    debugDraw3D.setPickingID(instance.pickingID);
                    debugDraw3D.setColor(color);
                    debugDraw3D.setTaper(instance.getTaper());
                    _cmdList->setGraphicRootConstants(0, (u32 *)&debugDraw3D, DebugDrawRootConstants3DCount);
                    _cmdList->drawIndexed(indexCount);
                }
            }

            // Opaque
            {
                setupZPass(_cmdList);

                for (uint i = 0; i < instances.size(); ++i)
                {
                    const T & instance = instances[i];
                    float4 color = unpackRGBA8(instance.color) * opaqueColor;

                    debugDraw3D.setWorldMatrix(transpose(instance.world));
                    debugDraw3D.setPickingID(instance.pickingID);
                    debugDraw3D.setColor(color);
                    debugDraw3D.setTaper(instance.getTaper());
                    _cmdList->setGraphicRootConstants(0, (u32 *)&debugDraw3D, DebugDrawRootConstants3DCount);
                    _cmdList->drawIndexed(indexCount);
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::drawCube(gfx::CommandList * _cmdList, DebugDrawFillMode _mode, bool _zTest, const core::float3 & _minPos, const core::float3 & _maxPos, core::u32 _color, const core::float4x4 & _matrix, PickingID _pickingID) const
    {
        drawDebugModelInstance(_cmdList, _mode, _zTest, m_cube, getCubeMatrix(_matrix, _minPos, _maxPos), _color, 1.0f, _pickingID);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::drawSphere(gfx::CommandList * _cmdList, DebugDrawFillMode _mode, bool _zTest, float _radius, core::u32 _color, const core::float4x4 & _matrix, PickingID _pickingID) const
    {
        drawDebugModelInstance(_cmdList, _mode, _zTest, m_icoSphere, getIcoSphereMatrix(_matrix, _radius), _color, 1.0f, _pickingID);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::drawCylinder(gfx::CommandList * _cmdList, DebugDrawFillMode _mode, bool _zTest, float _radius, float _height, core::u32 _color, const core::float4x4 & _matrix, PickingID _pickingID) const
    {
        drawDebugModelInstance(_cmdList, _mode, _zTest, m_cylinder, getCylinderMatrix(_matrix, _radius, _height), _color, 1.0f, _pickingID);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::drawSquarePyramid(gfx::CommandList * _cmdList, DebugDrawFillMode _mode, bool _zTest, float _base, float _height, core::u32 _color, const core::float4x4 & _matrix, PickingID _pickingID) const
    {
        drawDebugModelInstance(_cmdList, _mode, _zTest, m_squarePyramid, getSquarePyramidMatrix(_matrix, _base, _height), _color, 1.0f, _pickingID);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::drawDebugModelInstance(gfx::CommandList * _cmdList, DebugDrawFillMode _mode, bool _zTest, const MeshGeometry * _geometry, const float4x4 & _matrix, u32 _color, float _taper, PickingID _pickingID) const
    {
        DebugDrawRootConstants3D debugDraw3D;
        uint indexCount;

        if (setupDebugModelInstances(_cmdList, _geometry, _mode, debugDraw3D, indexCount))
        {
            debugDraw3D.setWorldMatrix(transpose(_matrix));
            debugDraw3D.setPickingID(_pickingID);
            debugDraw3D.setTaper(_taper);

            const float4 color = unpackRGBA8(_color);

            if (_mode == DebugDrawFillMode::Outline)
            {
                setupOutline(_cmdList);

                _cmdList->setGraphicRootConstants(0, (u32 *)&debugDraw3D, DebugDrawRootConstants3DCount);
                _cmdList->drawIndexed(indexCount);
            }
            else
            {
                if (!_zTest)
                {
                    setupZFail(_cmdList);
                    debugDraw3D.setColor(color * transparentColor);
                    _cmdList->setGraphicRootConstants(0, (u32 *)&debugDraw3D, DebugDrawRootConstants3DCount);
                    _cmdList->drawIndexed(indexCount);
                }
                else
                {
                    setupZPass(_cmdList);
                    debugDraw3D.setColor(color * opaqueColor);
                    _cmdList->setGraphicRootConstants(0, (u32 *)&debugDraw3D, DebugDrawRootConstants3DCount);
                    _cmdList->drawIndexed(indexCount);
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::setupZFail(gfx::CommandList * _cmdList) const
    {
        BlendState bsAlpha(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha, BlendOp::Add);
        _cmdList->setBlendState(bsAlpha);

        DepthStencilState dsAlpha(false, false, ComparisonFunc::Always);
        _cmdList->setDepthStencilState(dsAlpha);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::setupZPass(gfx::CommandList * _cmdList) const
    {
        BlendState bsOpaque(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha, BlendOp::Add);
        _cmdList->setBlendState(bsOpaque);

        DepthStencilState dsOpaque(true, true, ComparisonFunc::LessEqual);
        _cmdList->setDepthStencilState(dsOpaque);
    }

    //--------------------------------------------------------------------------------------
    void DebugDraw::setupOutline(gfx::CommandList * _cmdList) const
    {
        BlendState bsAlpha(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);
        _cmdList->setBlendState(bsAlpha);

        DepthStencilState dsOpaque(true, true, ComparisonFunc::LessEqual);
        _cmdList->setDepthStencilState(dsOpaque);
    }
}