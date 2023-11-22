#pragma once

#include "MeshImporterData.h"
#include "AnimImporterData.h"

namespace vg::renderer
{
    class SceneImporterData
    {
    public:
        core::vector<MeshImporterData>  meshes;
        core::vector<AnimImporterData>  anims;
    };
}