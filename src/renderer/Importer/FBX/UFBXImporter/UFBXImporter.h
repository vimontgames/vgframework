#pragma once

#include "renderer/Importer/FBX/FBXImporter.h"

struct ufbx_mesh;

namespace vg::renderer
{
    class SceneImporterData;
    class MeshImporterData;

    class UFBXImporter : public FBXImporter
    {
    public:
        UFBXImporter();
        ~UFBXImporter();

        bool importFBX(const core::string & _path, SceneImporterData & _data) final override;

    private:
        bool loadFBXMesh(const ufbx_mesh * _UFBXMesh, MeshImporterData & _data, double _scale);
    };
}