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

    class FBXImporter : public gfx::Importer, public core::Singleton<FBXImporter>
    {
    public:
        FBXImporter();
        ~FBXImporter();

        bool importFBX(const core::string & _path, SceneImporterData & _data);

    private:
        bool loadFBXMesh(fbxsdk::FbxNode * _fbxNode, fbxsdk::FbxMesh * _fbxMesh, MeshImporterData & _data, double _scale);
        template <class T, class V> bool importFBXLayerElement(core::vector<V> & _result, const fbxsdk::FbxMesh * _fbxMesh, T * _layerElement, core::uint _triangleCount, V const & _default);

    private:
        fbxsdk::FbxManager * m_fbxManager = nullptr;
    };
}