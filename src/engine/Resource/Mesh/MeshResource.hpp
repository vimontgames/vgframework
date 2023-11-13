#include "MeshResource.h"
#include "core/File/File.h"
#include "renderer/IMeshModel.h"
#include "renderer/IMaterialModel.h"
#include "renderer/Model/Material/Material_Consts.h"

using namespace vg::core;

namespace vg::engine
{
    VG_AUTO_REGISTER_CLASS(MeshResource);

    //--------------------------------------------------------------------------------------
    bool MeshResource::registerClass(IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(MeshResource, "Mesh Resource", IClassDesc::Flags::Resource))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MeshResource::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerPropertyObjectVectorHelper(MeshResource, m_materialResources, MaterialResource, "Materials", IProperty::Flags::NotSaved | IProperty::Flags::ReadOnly);

        return true;
    }

    //--------------------------------------------------------------------------------------
    MeshResource::MeshResource(const core::string & _name, IObject * _parent) :
        Resource(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    MeshResource::~MeshResource()
    {
        // unload this
        ResourceManager::get()->unloadResource(this, GetResourcePath());
    }

    //--------------------------------------------------------------------------------------
    const core::vector<core::string> MeshResource::getExtensions() const
    {
        vector<string> ext;
                       ext.push_back(".fbx");
        return ext;
    }

    //--------------------------------------------------------------------------------------
    void MeshResource::onResourcePathChanged(const string & _oldPath, const string & _newPath)
    {
        if (_oldPath != _newPath)
            ResourceManager::get()->loadResourceAsync(this, _oldPath, _newPath);
    }

    //--------------------------------------------------------------------------------------
    bool MeshResource::cook(const string & _file) const
    {
        auto * renderer = Engine::get()->GetRenderer();
        return renderer->cookMeshModel(_file);
    }

    //--------------------------------------------------------------------------------------
    core::IObject * MeshResource::load(const string & _path)
    {
        auto * renderer = Engine::get()->GetRenderer();
        auto * meshModel = renderer->loadMeshModel(_path);    
        return meshModel;
    }

    //--------------------------------------------------------------------------------------
    void MeshResource::unload(const core::string & _path)
    {
        super::unload(_path);
    }

    //--------------------------------------------------------------------------------------
    void MeshResource::onResourceLoaded(core::IResource * _resource)
    {
        const auto userData = _resource->getUserData();
        const uint matID = (userData >> 16) & 0xFFFF;
        const auto texSlot = (renderer::MaterialTextureType)(userData & 0xFFFF);

        auto * meshModel = getMeshModel();
        auto * material = meshModel->GetMaterial(matID);
        auto * texRes = (TextureResource *)_resource;

        material->SetTexture(texSlot, texRes->getTexture());
    }

    //--------------------------------------------------------------------------------------
    void MeshResource::onResourceUnloaded(core::IResource * _resource)
    {
        const auto userData = _resource->getUserData();
        const uint matID = (userData >> 16) & 0xFFFF;
        const auto texSlot = (renderer::MaterialTextureType)(userData & 0xFFFF);

        auto * meshModel = getMeshModel();
        auto * material = meshModel->GetMaterial(matID);

        material->SetTexture(texSlot, nullptr);
    }

    //--------------------------------------------------------------------------------------
    void MeshResource::loadSubResources()
    {
        auto * meshModel = dynamic_cast<renderer::IMeshModel *>(getObject());
        VG_ASSERT(nullptr != meshModel);

        if (nullptr != meshModel)
        {
            const uint matCount = meshModel->GetMaterialCount();
            m_materialResources.resize(matCount);
            for (uint m = 0; m < matCount; ++m)
            {
                renderer::IMaterialModel * matModel = meshModel->GetMaterial(m);
                if (nullptr != matModel)
                {
                    const auto texCount = matModel->GetTextureCount();
                    MaterialResource & matRes = m_materialResources[m];
                    matRes.setObject(matModel);
                    matRes.setName(matModel->getName());
                    matRes.m_textureResources.resize(texCount);

                    for (uint t = 0; t < texCount; ++t)
                    {
                        string matTexPath = matModel->GetTexturePath((renderer::MaterialTextureType)t);

                        if (matTexPath.length() > 0)
                        {
                            matTexPath = matTexPath;
                            TextureResource & res = matRes.m_textureResources[t];

                            string name = core::asString((renderer::MaterialTextureType)t);
                            res.setName(name.c_str());
                            res.setup(this, matTexPath, m << 16 | t);    // will trigger loading of textures
                        }
                    }
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    // Textures are currently SubResources from MeshResources (In the future, textures should be SubResources of MaterialResources, and Materials should be SubResources of MeshResources)
    // When a MeshResource (<=> a handle to shared MeshModel data) is unloaded we "detach" the associated Texture Resources
    //--------------------------------------------------------------------------------------
    void MeshResource::unloadSubResources()
    {
        auto * meshModel = dynamic_cast<renderer::IMeshModel *>(getObject());
        VG_ASSERT(nullptr != meshModel);

        if (nullptr != meshModel)
        {
            const uint matCount = meshModel->GetMaterialCount();
            for (uint m = 0; m < m_materialResources.size(); ++m)
            {
                MaterialResource & matRes = m_materialResources[m];
            
                for (uint t = 0; t < matRes.m_textureResources.size(); ++t)
                {
                    TextureResource & res = matRes.m_textureResources[t];
                    res.ClearResourcePath(); 
                }
            }
        }
        m_materialResources.clear();
    }
}