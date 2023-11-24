#pragma once

#include "core/Math/Math.h"
#include "MaterialImporterData.h"
#include "renderer/Geometry/Batch/Batch.h"
#include "renderer/Geometry/AABB/AABB.h"
#include "renderer/Geometry/Vertex/VertexFormat.h"

namespace vg::renderer
{
    using MeshImporterVertex = renderer::FatVertex;

    class MeshImporterNode 
    {
    public:
        bool read(core::io::Buffer & _buffer);
        bool write(core::io::Buffer & _buffer) const;

        core::string name;
        core::u32 parent_index;
        core::float4x4 geometry_to_node;
        core::float4x4 node_to_parent;
        core::float4x4 node_to_world;
        core::float4x4 geometry_to_world;
        core::float4x4 normal_to_world;
    };

    class MeshImporterData
    {
    public:
        bool load(const core::string & _file);
        bool save(const core::string & _file) const;

        core::string                        name;
        AABB                                aabb;
        core::vector<Batch>                 batches;
        core::vector<core::u32>             indices;
        core::vector<MeshImporterVertex>    vertices;
        core::vector<MaterialImporterData>  materials;
        u8                                  maxBonesCountPerVertex;
        core::vector<MeshImporterNode>      nodes;
        core::vector<core::u32>             bonesIndices;
        core::vector<core::float4x4>        bonesMatrices;
    };
}