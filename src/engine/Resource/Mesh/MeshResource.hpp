#include "MeshResource.h"
#include "graphics/renderer/IMeshModel.h"
#include "graphics/renderer/IMaterialModel.h"

using namespace vg::core;

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    MeshResource::MeshResource(IObject * _owner) :
        Resource(_owner)
    {

    }

    //--------------------------------------------------------------------------------------
    MeshResource::~MeshResource()
    {

    }

    //--------------------------------------------------------------------------------------
    const core::vector<core::string> MeshResource::getExtensions() const
    {
        vector<string> ext;
        ext.push_back(".fbx");
        return ext;
    }

    //--------------------------------------------------------------------------------------
    void MeshResource::onPathChanged(IObject * _owner, const string & _oldPath, const string & _newPath)
    {
        if (_oldPath != _newPath)
            ResourceManager::get()->loadResourceAsync(this, _newPath, _owner);
    }

    //--------------------------------------------------------------------------------------
    core::string extractFolder(const string & _path)
    {
        auto found = _path.find_last_of('/');
        if (core::string::npos != found)
        {
            string folder = _path.substr(0, found);
            return folder + "/";
        }
        else
            return _path;
    }

    //--------------------------------------------------------------------------------------
    bool MeshResource::loadResource(const core::string & _path, core::IObject * _owner)
    {
        auto * engine = Engine::get();
        auto * renderer = engine->getRenderer();

        VG_SAFE_RELEASE(m_object);

        auto * meshModel = renderer->createMeshModel(_path);

        const auto matCount = meshModel->GetMaterialCount();
        m_materialResources.resize(matCount);
        for (uint m = 0; m < matCount; ++m)
        {
            auto * matModel = meshModel->GetMaterial(m);
            if (nullptr != matModel)
            {
                const auto texCount = matModel->GetTextureCount();
                auto & matRes = m_materialResources[m];
                matRes.m_textureResources.resize(texCount);

                for (uint t = 0; t < texCount; ++t)
                {
                    string matTexPath = matModel->GetTexturePath((graphics::renderer::MaterialTextureType)t);
                    
                    if (matTexPath.length() > 0)
                    {
                        matTexPath = extractFolder(_path) + matTexPath;
                        auto & res = matRes.m_textureResources[t];

                        res.setup(this, matTexPath, m << 16 | t);    // will trigger loading because path changed
                    }
                }
            }
        }

        m_object = meshModel;
        return nullptr != m_object;
    }

    //--------------------------------------------------------------------------------------
    void MeshResource::onResourceLoaded(core::IResource * _resource)
    {
        const auto userData = _resource->getUserData();
        const uint matID =   (userData >> 16) & 0xFFFF;
        const auto texSlot = (graphics::renderer::MaterialTextureType)(userData & 0xFFFF);

        auto * meshModel = getMeshModel();
        auto * material = meshModel->GetMaterial(matID);

        auto * texRes = (TextureResource*)_resource;

        material->SetTexture(texSlot, texRes->getTexture());
    }
}