#pragma once

#include "renderer/Importer/FBX/FBXImporter.h"

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

    class FBXSDKImporter : public FBXImporter
    {
    public:
        FBXSDKImporter();
        ~FBXSDKImporter();

        bool importFBX(const core::string & _path, SceneImporterData & _data) final override;

    private:
        bool loadFBXMesh(fbxsdk::FbxNode * _fbxNode, fbxsdk::FbxMesh * _fbxMesh, MeshImporterData & _data, double _scale);
        template <class T, class V> bool importFBXLayerElement(core::vector<V> & _result, const fbxsdk::FbxMesh * _fbxMesh, T * _layerElement, core::uint _triangleCount, V const & _default);

    private:
        fbxsdk::FbxManager * m_fbxManager = nullptr;
    };
}