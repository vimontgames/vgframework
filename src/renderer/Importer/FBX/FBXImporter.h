#pragma once

#include "gfx/Importer/Importer.h"
#include "core/Singleton/Singleton.h"

namespace fbxsdk
{
    class FbxManager;
    class FbxNode;
    class FbxMesh;
}

namespace vg::gfx
{
    struct MeshImporterSettings;
}

namespace vg::renderer
{
    class SceneImporterData;
    class MeshImporterData;

    class FBXImporter : public gfx::Importer
    {
    public:
        virtual bool ImportFBX(const core::string & _path, SceneImporterData & _data, const gfx::MeshImporterSettings * _meshImportSettings) = 0;
    };
}