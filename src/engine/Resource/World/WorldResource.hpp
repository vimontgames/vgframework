#include "WorldResource.h"
#include "core/File/File.h"
#include "core/IWorld.h"
#include "engine/Resource/World/WorldResourceData.hpp"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_RESOURCE_CLASS(WorldResource, "Scene");

    //--------------------------------------------------------------------------------------
    void * WorldResource::ResizeWorldResourceVector(IObject * _parent, uint _offset, uint _count, uint & _elementSize)
    {
        auto vec = (core::vector<WorldResource> *)(uint_ptr(_parent) + _offset);
        vec->clear();
        vec->resize(_count);
        _elementSize = sizeof(WorldResource);
        return vec->data();
    }

    //--------------------------------------------------------------------------------------
    bool WorldResource::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerResizeVectorFunc(WorldResource, ResizeWorldResourceVector);

        return true;
    }

    //--------------------------------------------------------------------------------------
    WorldResource::WorldResource(const core::string & _name, IObject * _parent) :
        super(_name, _parent)
    {
       
    }

    //--------------------------------------------------------------------------------------
    WorldResource::~WorldResource()
    {
        ResourceManager::get()->unloadResource(this, GetResourcePath());
    }

    //--------------------------------------------------------------------------------------
    const core::vector<core::string> WorldResource::getExtensions() const
    {
        vector<string> ext;
        ext.push_back(".world");
        return ext;
    }

    //--------------------------------------------------------------------------------------
    void WorldResource::onResourcePathChanged(const string & _oldPath, const string & _newPath)
    {
        if (_oldPath != _newPath)
            ResourceManager::get()->loadResourceAsync(this, _oldPath, _newPath);
    }

    //--------------------------------------------------------------------------------------
    bool WorldResource::cook(const string & _file) const
    {
        // Cooked file is same format as source file for now (TODO : serializeFrom/ToBinary)
        const string cookedPath = io::getCookedPath(_file);

        string data;
        if (io::readFile(_file, data))
        {
            if (io::writeFile(cookedPath, data))
            {
                return true;
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    core::IObject * WorldResource::load(const string & _path)
    {
        IFactory * factory = Kernel::getFactory();
        WorldResourceData * worldData = dynamic_cast<WorldResourceData *>(factory->createObject("WorldResourceData"));
        if (nullptr != worldData)
        {
            worldData->setParent(this);
            if (factory->loadFromXML(worldData, _path))
            {
                worldData->createWorld();

                return worldData;
            }
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    // a .scene file is an XML file that serializes a Scene Object
    //--------------------------------------------------------------------------------------
    bool WorldResource::CreateFile(const core::string & _path)
    {
        const auto * factory = Kernel::getFactory();

        WorldResourceData * worldData = dynamic_cast<WorldResourceData *>(factory->createObject("WorldResourceData", core::io::getFileNameWithoutExt(_path)));
        if (nullptr != worldData)
        {
            factory->saveToXML(worldData, _path);
            VG_SAFE_RELEASE(worldData);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool WorldResource::SaveFile(const string & _path) const
    {
        WorldResourceData * worldData = dynamic_cast<WorldResourceData *>(getObject());
        if (nullptr != worldData)
        {
            const auto * factory = Kernel::getFactory();
            return factory->saveToXML(worldData, _path);
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    core::IWorld * WorldResource::GetWorld() const
    {
        WorldResourceData * worldResData = dynamic_cast<WorldResourceData *>(getObject());
        if (worldResData)
            return dynamic_cast<IWorld *>(worldResData->getWorld());
        
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    void WorldResource::CreateSceneResource(const core::string & _file)
    {
        WorldResourceData * worldResData = dynamic_cast<WorldResourceData *>(getObject());
        if (worldResData)
        {
            VG_ASSERT(worldResData->m_sceneResources.size() + 1 < MAX_SCENE_PER_WORLD);
            SceneResource * sceneRes = new SceneResource();
            sceneRes->setName(io::getFileNameWithoutExt(_file));

            if (sceneRes->CreateFile(_file))
            {
                sceneRes->setParent(this);
                sceneRes->SetResourcePath(_file);
                worldResData->m_sceneResources.push_back(sceneRes);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void WorldResource::LoadSceneResource(const core::string & _file)
    {
        WorldResourceData * worldResData = dynamic_cast<WorldResourceData *>(getObject());
        if (worldResData)
        {
            VG_ASSERT(worldResData->m_sceneResources.size() + 1 < MAX_SCENE_PER_WORLD);
            SceneResource * sceneRes = new SceneResource();
            sceneRes->setParent(worldResData);
            sceneRes->SetResourcePath(_file);
            worldResData->m_sceneResources.push_back(sceneRes);
        }        
    }

    //--------------------------------------------------------------------------------------
    IResource * WorldResource::FindSceneResource(core::IScene * _scene)
    {
        WorldResourceData * worldResData = dynamic_cast<WorldResourceData *>(getObject());
        if (worldResData)
        {
            for (uint i = 0; i < worldResData->m_sceneResources.size(); ++i)
            {
                if (worldResData->m_sceneResources[i]->getObject() == _scene)
                    return worldResData->m_sceneResources[i];
            }
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    void WorldResource::UnloadSceneResource(core::IResource * _resource)
    {
        WorldResourceData * worldResData = dynamic_cast<WorldResourceData *>(getObject());
        if (worldResData)
            VG_SAFE_RELEASE(_resource);
    }

    //--------------------------------------------------------------------------------------
    core::uint WorldResource::GetSceneResourceCount() const
    {
        WorldResourceData * worldResData = dynamic_cast<WorldResourceData *>(getObject());
        if (worldResData)
            return (core::uint)worldResData->m_sceneResources.size();

        return 0;
    }

    //--------------------------------------------------------------------------------------
    core::IResource * WorldResource::GetSceneResource(core::uint _index) const
    {
        IObject * obj = getObject();
        VG_ASSERT(nullptr == obj || dynamic_cast<WorldResourceData*>(obj));
        WorldResourceData * worldResData = (WorldResourceData *)obj;
        if (worldResData)
        {
            if (_index < GetSceneResourceCount())
            {
                IResource * res = worldResData->m_sceneResources[_index];
                VG_ASSERT(dynamic_cast<core::IResource*>(res));
                return (core::IResource *)res;
            }
        }
        
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    void WorldResource::onResourceLoaded(core::IResource * _resource)
    {
        WorldResourceData * worldResData = dynamic_cast<WorldResourceData *>(getObject());
        VG_ASSERT(worldResData);

        for (uint i = 0; i < worldResData->m_sceneResources.size(); ++i)
        {
            if (_resource == worldResData->m_sceneResources[i])
            {
                IScene * scene = dynamic_cast<IScene *>(_resource->getObject());
                if (scene)
                {
                    if (worldResData->m_world)
                    {
                        if (worldResData->m_world->AddScene(scene))
                        {
                            if (Engine::get()->isPlaying())
                            {
                                IObject * root = scene->GetRoot();
                                if (nullptr != root)
                                    root->OnPlay();
                            }
                        }
                    }
                }
            }
        }        
    }

    //--------------------------------------------------------------------------------------
    void WorldResource::onResourceUnloaded(core::IResource * _resource)
    {
        WorldResourceData * worldResData = dynamic_cast<WorldResourceData *>(getObject());
        VG_ASSERT(worldResData);

        for (uint i = 0; i < worldResData->m_sceneResources.size(); ++i)
        {
            if (_resource == worldResData->m_sceneResources[i])
            {
                IScene * scene = dynamic_cast<IScene *>(_resource->getObject());
                if (worldResData->m_world)
                {
                    worldResData->m_world->RemoveScene(scene);
                    worldResData->m_sceneResources.remove((SceneResource *)_resource);
                    return;
                }
            }
        }
    }
}