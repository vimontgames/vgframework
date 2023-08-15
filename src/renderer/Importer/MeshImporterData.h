#pragma once

#include "MaterialImporterData.h"
#include "shaders/system/vertex.hlsl.h"
#include "renderer/Geometry/Batch/Batch.h"
#include "renderer/Geometry/AABB/AABB.h"

namespace vg::renderer
{
    using MeshImporterVertex = gfx::FatVertex;

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
    };
}