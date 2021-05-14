#pragma once

#include "Shaders/system/vertex.hlsl.h"
#include "graphics/renderer/Geometry/Batch.h"

namespace vg::graphics::renderer
{
    using MeshImporterVertex = driver::FatVertex;

    class MeshImporterData
    {
    public:
        core::vector<Batch>                 batches;
        core::vector<core::u32>             indices;
        core::vector<MeshImporterVertex>    vertices;
    };

    class SceneImporterData
    {
    public:
        core::vector<MeshImporterData>      meshes;
    };
}