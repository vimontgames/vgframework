#include "PrefabResource.h"
#include "core/File/File.h"
#include "core/IBaseScene.h"

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
        ResourceManager::get()->unloadResource(this, GetResourcePath());
    }

    //--------------------------------------------------------------------------------------
    const core::vector<core::string> PrefabResource::getExtensions() const
    {
        vector<string> ext;
        ext.push_back(".prefab");
        return ext;
    }

    //--------------------------------------------------------------------------------------
    core::IObject* PrefabResource::load(const string& _path)
    {
        IFactory* factory = Kernel::getFactory();
        IBaseScene* scene = dynamic_cast<IBaseScene*>(factory->createObject("Scene"));
        if (nullptr != scene)
        {
            scene->setParent(this);
            if (factory->loadFromXML(scene, _path))
                return scene;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    // a .scene file is an XML file that serializes a Scene Object
    //--------------------------------------------------------------------------------------
    bool PrefabResource::CreateFile(const core::string& _path)
    {
        const auto* factory = Kernel::getFactory();

        IBaseScene* scene = dynamic_cast<IBaseScene*>(factory->createObject("Scene"));
        if (nullptr != scene)
        {
            // Use file name as default scene name
            scene->setName(io::getFileNameWithoutExt(_path));

            // Add default root node to scene
            GameObject* root = (GameObject*)CreateFactoryObject(GameObject, "Root", this);
            scene->SetRoot(root);
            root->release();

            factory->saveToXML(scene, _path);
            VG_SAFE_RELEASE(scene);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool PrefabResource::SaveFile(const string& _path) const
    {
        IBaseScene* scene = dynamic_cast<IBaseScene*>(getObject());
        if (nullptr != scene)
        {
            const auto* factory = Kernel::getFactory();
            return factory->saveToXML(scene, _path);
        }
        return false;
    }
}