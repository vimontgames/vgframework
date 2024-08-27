#include "PrefabResource.h"
#include "core/File/File.h"
#include "core/IBaseScene.h"
#include "engine/World/Prefab/PrefabScene.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_RESOURCE_CLASS(PrefabResource, "Prefab");

    //--------------------------------------------------------------------------------------
    void* PrefabResource::ResizePrefabResourceVector(IObject* _parent, uint _offset, uint _count, uint& _elementSize)
    {
        auto vec = (core::vector<PrefabResource> *)(uint_ptr(_parent) + _offset);
        vec->clear();
        vec->resize(_count);
        _elementSize = sizeof(PrefabResource);
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
        PrefabScene * prefabScene = VG_SAFE_STATIC_CAST(PrefabScene, factory->createObject("PrefabScene"));
        if (nullptr != prefabScene)
        {
            prefabScene->SetParent(this);
            if (factory->loadFromXML(prefabScene, _path))
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

        if (PrefabScene * prefabScene = VG_SAFE_STATIC_CAST(PrefabScene, factory->createObject("PrefabScene")))
        {
            // Use file name as default scene name
            prefabScene->setName(io::getFileNameWithoutExt(_path));

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

            factory->saveToXML(prefabScene, _path);
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
            return factory->saveToXML(scene, _path);
        }
        return false;
    }
}