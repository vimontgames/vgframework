#pragma once

#include "Shaders/system/vertex.hlsl.h"
#include "graphics/renderer/Geometry/Batch.h"

namespace vg::graphics::renderer
{
    using MeshImporterVertex = driver::FatVertex;

    enum class MaterialImporterTexture : core::u8
    {
        Albedo = 0,
        Normal
    };

    class MaterialImporterData
    {
    public:
        core::string name;
        core::string texturePath[core::enumCount<MaterialImporterTexture>()];
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