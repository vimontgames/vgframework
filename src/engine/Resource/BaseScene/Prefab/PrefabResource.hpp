#include "PrefabResource.h"
#include "core/File/File.h"
#include "core/IBaseScene.h"
#include "engine/World/Prefab/PrefabScene.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_RESOURCE_CLASS(PrefabResource, "Prefab");

    //--------------------------------------------------------------------------------------
    void* ResizePrefabResourceVector(IObject* _parent, uint _offset, uint _count)
    {
        auto vec = (core::vector<PrefabResource> *)(uint_ptr(_parent) + _offset);
        vec->clear();
        vec->resize(_count);
        return vec->data();
    }

    //--------------------------------------------------------------------------------------
    bool PrefabResource::registerProperties(IClassDesc& _desc)
    {
        super::registerProperties(_desc);

        registerResizeVectorFunc(PrefabResource, ResizePrefabResourceVector);

        return true;
    }

    //--------------------------------------------------------------------------------------
    PrefabResource::PrefabResource(const core::string& _name, IObject* _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    PrefabResource::~PrefabResource()
    {
        
    }

    //--------------------------------------------------------------------------------------
    const core::vector<core::string> PrefabResource::GetExtensions() const
    {
        vector<string> ext;
        ext.push_back(".prefab");
        return ext;
    }

    //--------------------------------------------------------------------------------------
    core::IObject* PrefabResource::Load(const string& _path)
    {
        IFactory* factory = Kernel::getFactory();
        PrefabScene * prefabScene = VG_SAFE_STATIC_CAST(PrefabScene, factory->CreateObject("PrefabScene"));
        if (nullptr != prefabScene)
        {
            prefabScene->SetParent(nullptr/* this*/);
            prefabScene->SetName(io::getFileName(_path));

            if (factory->LoadFromXML(prefabScene, _path))
                return prefabScene;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    // a .scene file is an XML file that serializes a Scene Object
    //--------------------------------------------------------------------------------------
    bool PrefabResource::CreateFile(const core::string& _path, core::IObject * _data)
    {
        auto * factory = Kernel::getFactory();

        if (PrefabScene * prefabScene = VG_SAFE_STATIC_CAST(PrefabScene, factory->CreateObject("PrefabScene")))
        {
            // Use file name as default scene name
            prefabScene->SetName(io::getFileNameWithoutExt(_path));

            // Add default root node to scene
            auto root = (GameObject *)CreateFactoryObject(GameObject, "Root", this);
            prefabScene->SetRoot(root);

            // Add child if provided
            if (_data)
            {
                IGameObject * srcGO = VG_SAFE_STATIC_CAST(IGameObject, _data);
                //IGameObject * newGameObject = (IGameObject *)factory->createObject("GameObject");
                //factory->CopyProperties(srcGO, newGameObject);

                root->AddChild(srcGO);
                //VG_SAFE_RELEASE(newGameObject);
            }

            VG_SAFE_RELEASE(root);

            factory->SaveToXML(prefabScene, _path);
            VG_SAFE_RELEASE(prefabScene);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool PrefabResource::SaveFile(const string& _path) const
    {
        IBaseScene* scene = dynamic_cast<IBaseScene*>(GetObject());
        if (nullptr != scene)
        {
            const auto* factory = Kernel::getFactory();
            return factory->SaveToXML(scene, _path);
        }
        return false;
    }
}