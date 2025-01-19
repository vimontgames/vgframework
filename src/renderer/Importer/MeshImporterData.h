#pragma once

#include "MaterialImporterData.h"
#include "CollisionImporterData.h"
#include "core/Math/Math.h"
#include "core/Misc/AABB/AABB.h"
#include "renderer/Geometry/Batch/Batch.h"
#include "renderer/Geometry/Vertex/VertexFormat.h"
#include "BodyPartFlags.h"

namespace vg::renderer
{
    using MeshImporterVertex = renderer::FatVertex;

    struct BonesNamingConvention
    {
        core::string name;
        BodyPartFlags flags;
    };

    class MeshImporterNode 
    {
    public:
        bool read(core::io::Buffer & _buffer);
        bool write(core::io::Buffer & _buffer) const;

        bool computeBodyPartFlags();

        bool                selected = false;

        core::string        name;
        core::i16           parent_index;
        BodyPartFlags       flags;

        core::float4x4      geometry_to_node;
        core::float4x4      node_to_parent;
        core::float4x4      node_to_world;
        core::float4x4      geometry_to_world;
        core::float4x4      normal_to_world;

        core::quaternion    rot;
        core::float3        pos;
        core::float3        scale;
    };

    class MeshImporterData
    {
    public:
        bool load(const core::string & _file);
        bool save(const core::string & _file) const;

        core::string                        name;
        core::AABB                          aabb;
        core::vector<Batch>                 batches;
        core::vector<core::u32>             indices;
        core::vector<MeshImporterVertex>    vertices;
        core::vector<MaterialImporterData>  materials;
        u8                                  maxBonesCountPerVertex;
        core::vector<MeshImporterNode>      nodes;
        core::vector<core::u32>             bonesIndices;
        core::vector<core::float4x4>        bonesMatrices;
        core::vector<ColliderTriangle>      colliderTriangles;
    };
}