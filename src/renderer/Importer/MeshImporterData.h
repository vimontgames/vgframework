#pragma once

#include "MaterialImporterData.h"
#include "renderer/Geometry/Batch/Batch.h"
#include "renderer/Geometry/AABB/AABB.h"
#include "renderer/Geometry/Vertex/VertexFormat.h"

namespace vg::renderer
{
    using MeshImporterVertex = renderer::FatVertex;

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
}