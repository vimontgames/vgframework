#pragma once

#include "MaterialImporterData.h"
#include "Shaders/system/vertex.hlsl.h"
#include "graphics/renderer/Geometry/Batch.h"

namespace vg::graphics::renderer
{
    using MeshImporterVertex = driver::FatVertex;

    class MeshImporterData
    {
    public:
        bool load(const core::string & _file);
        bool save(const core::string & _file) const;

        core::vector<Batch>                 batches;
        core::vector<core::u32>             indices;
        core::vector<MeshImporterVertex>    vertices;
        core::vector<MaterialImporterData>  materials;
    };
}