#pragma once

#include "MaterialImporterData.h"
#include "renderer/Geometry/Batch/Batch.h"
#include "renderer/Geometry/AABB/AABB.h"
#include "renderer/Geometry/Vertex/VertexFormat.h"

namespace vg::renderer
{
    using MeshImporterVertex = renderer::FatVertex;

    template <core::uint N> struct SkinVertex
    {
        core::u8 bone_index[N];
        core::u8 bone_weight[N];
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
        core::vector<core::u32>             bonesIndices;
        core::vector<core::float4x4>        bonesMatrices;
    };

    class AnimNodeData
    {
    public:
        float                           time_begin;
        float                           framerate;
        core::uint                      num_frames;
        core::quaternion                const_rot;
        core::float3                    const_pos;
        core::float3                    const_scale;
        core::vector<core::quaternion>  rot;
        core::vector<core::float3>      pos;
        core::vector<core::float3>      scale;
    };;

    class AnimImporterData
    {
    public:
        bool load(const core::string & _file);
        bool save(const core::string & _file) const;

        core::string                name;
        float                       time_begin;
        float                       time_end;
        float                       framerate;
        uint                        num_frames;
        core::vector<AnimNodeData>  animNodes;
    };
}