#pragma once

#include "renderer/Importer/FBX/FBXImporter.h"

struct ufbx_scene;
struct ufbx_mesh;
struct ufbx_anim_stack;
struct ufbx_node;

namespace vg::renderer
{
    class SceneImporterData;
    class MeshImporterData;
    class AnimImporterData;
    class AnimNodeData;

    class UFBXImporter : public FBXImporter
    {
    public:
        UFBXImporter();
        ~UFBXImporter();

        bool ImportFBX(const core::string & _path, SceneImporterData & _data, const gfx::MeshImporterSettings * _meshImportSettings) final override;

    private:
        bool loadFBXMesh(const ufbx_scene * _UFBXScene, const ufbx_mesh * _UFBXMesh, MeshImporterData & _data, double _scale, const gfx::MeshImporterSettings * _meshImportSettings);
        bool loadFBXAnim(const ufbx_scene * _UFBXScene, const ufbx_anim_stack * _UFBXAnimStack, AnimImporterData & _animData);
        bool loadFBXAnimNode(const ufbx_anim_stack * _UFBXAnimStack, const ufbx_node * _UFBXNode, const AnimImporterData & _animData, AnimNodeData & _animNodeData);
    };
}