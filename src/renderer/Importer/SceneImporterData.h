#pragma once

#include "MeshImporterData.h"

namespace vg::renderer
{
    class SceneImporterData
    {
    public:
        core::vector<MeshImporterData>      meshes;
    };
}