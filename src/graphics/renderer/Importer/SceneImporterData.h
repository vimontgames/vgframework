#pragma once

#include "Shaders/system/vertex.hlsl.h"
#include "graphics/renderer/Geometry/Batch.h"
#include "graphics/renderer/Model/Material/MaterialTextureType.h"

namespace vg::graphics::renderer
{
    using MeshImporterVertex = driver::FatVertex;

    class MaterialImporterData
    {
    public:
        core::string name;
        core::string texturePath[core::enumCount<MaterialTextureType>()];
    };

    class MeshImporterData
    {
    public:
        core::vector<Batch>                 batches;
        core::vector<core::u32>             indices;
        core::vector<MeshImporterVertex>    vertices;
        core::vector<MaterialImporterData>  materials;
    };

    class SceneImporterData
    {
    public:
        core::vector<MeshImporterData>      meshes;
    };
}