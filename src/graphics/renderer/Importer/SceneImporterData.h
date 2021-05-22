#pragma once

#include "MeshImporterData.h"

namespace vg::graphics::renderer
{
    class SceneImporterData
    {
    public:
        core::vector<MeshImporterData>      meshes;
    };
}