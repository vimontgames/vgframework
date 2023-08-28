#pragma once

#include "gfx/Importer/Importer.h"
#include "core/Singleton/Singleton.h"

namespace fbxsdk
{
    class FbxManager;
    class FbxNode;
    class FbxMesh;
}

namespace vg::renderer
{
    class SceneImporterData;
    class MeshImporterData;

    class FBXImporter : public gfx::Importer
    {
    public:
        virtual bool importFBX(const core::string & _path, SceneImporterData & _data) = 0;
    };
}