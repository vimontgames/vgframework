#include "MeshResource.h"
#include "core/File/File.h"
#include "renderer/IMeshModel.h"
#include "renderer/IMaterialModel.h"
#include "renderer/Model/Material/Material_Consts.h"
#include "engine/Resource/Mesh/MeshResourceMeta.h"

#include "MeshResourceMeta.hpp"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_RESOURCE_CLASS(MeshResource, "Mesh", 100);

    //--------------------------------------------------------------------------------------
    bool MeshResource::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

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

    }

    //--------------------------------------------------------------------------------------
    const core::vector<core::string> MeshResource::GetExtensions() const
    {
        vector<string> ext;
                       ext.push_back(".fbx");
        return ext;
    }

    //--------------------------------------------------------------------------------------
    core::IResourceMeta * MeshResource::CreateResourceMeta(const core::string & _path) const
    {
        return new MeshResourceMeta(_path);
    }

    //--------------------------------------------------------------------------------------
    bool MeshResource::Cook(const string & _file) const
    {
        // Get meta if it exists
        auto * meta = (MeshResourceMeta *)ResourceManager::get()->GetOrCreateResourceMeta(this);

        auto * renderer = Engine::get()->GetRenderer();
        return renderer->CookMeshModel(_file, &meta->getMeshImportSettings());
    }

    //--------------------------------------------------------------------------------------
    core::IObject * MeshResource::Load(const string & _path)
    {
        auto * renderer = Engine::get()->GetRenderer();
        auto * meshModel = renderer->LoadMeshModel(_path);    
        return meshModel;
    }

    //--------------------------------------------------------------------------------------
    void MeshResource::Unload(const core::string & _path)
    {
        super::Unload(_path);
    }

    //--------------------------------------------------------------------------------------
    void MeshResource::OnResourceLoaded(core::IResource * _resource)
    {
        VG_PROFILE_CPU("MeshResource");

        const auto userData = _resource->GetUserData();
        const uint matID = (userData >> 16) & 0xFFFF;
        const auto texSlot = (renderer::MaterialTextureType)(userData & 0xFFFF);
    }

    //--------------------------------------------------------------------------------------
    void MeshResource::OnResourceUnloaded(core::IResource * _resource)
    {
        const auto userData = _resource->GetUserData();
        const uint matID = (userData >> 16) & 0xFFFF;
        const auto texSlot = (renderer::MaterialTextureType)(userData & 0xFFFF);
    }

    //--------------------------------------------------------------------------------------
    void MeshResource::LoadSubResources()
    {
        auto * meshModel = dynamic_cast<renderer::IMeshModel *>(GetObject());
        VG_ASSERT(nullptr != meshModel);
    }

    //--------------------------------------------------------------------------------------
    // Textures are currently SubResources from MeshResources (In the future, textures should be SubResources of MaterialResources, and Materials should be SubResources of MeshResources)
    // When a MeshResource (<=> a handle to shared MeshModel data) is unloaded we "detach" the associated Texture Resources
    //--------------------------------------------------------------------------------------
    void MeshResource::UnloadSubResources()
    {
        //auto * meshModel = dynamic_cast<renderer::IMeshModel *>(GetObject());
        //VG_ASSERT(nullptr != meshModel);
    }
}