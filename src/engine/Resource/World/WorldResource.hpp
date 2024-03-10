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
    bool WorldResource::CreateFile(const core::string & _path, core::IObject * _data)
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
    void WorldResource::CreateSceneResource(const core::string & _file, core::BaseSceneType _sceneType)
    {
        WorldResourceData * worldResData = dynamic_cast<WorldResourceData *>(getObject());
        if (worldResData)
        {
            switch (_sceneType)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(_sceneType);
                    break;

                case core::Scene:
                {
                    SceneResource * sceneRes = new SceneResource();
                    sceneRes->setName(io::getFileNameWithoutExt(_file));

                    if (sceneRes->CreateFile(_file))
                    {
                        sceneRes->setParent(this);
                        sceneRes->SetResourcePath(_file);
                        worldResData->m_sceneResources.push_back(sceneRes);
                    }
                }
                break;

                case core::Prefab:
                {
                    PrefabResource * prefabRes = new PrefabResource();
                    prefabRes->setName(io::getFileNameWithoutExt(_file));

                    if (prefabRes->CreateFile(_file))
                    {
                        prefabRes->setParent(this);
                        prefabRes->SetResourcePath(_file);
                        worldResData->m_prefabsResources.push_back(prefabRes);
                    }
                }
                break;
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void WorldResource::LoadSceneResource(const core::string & _file, core::BaseSceneType _sceneType)
    {
        WorldResourceData * worldResData = dynamic_cast<WorldResourceData *>(getObject());
        if (worldResData)
        {
            switch (_sceneType)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(_sceneType);
                    break;

                case core::Scene:
                {
                    SceneResource * sceneRes = new SceneResource();
                    sceneRes->setParent(worldResData);
                    sceneRes->SetResourcePath(_file);
                    worldResData->m_sceneResources.push_back(sceneRes);
                }
                break;

                case core::Prefab:
                {
                    PrefabResource * prefabRes = new PrefabResource();
                    prefabRes->setParent(worldResData);
                    prefabRes->SetResourcePath(_file);
                    worldResData->m_prefabsResources.push_back(prefabRes);
                }
                break;
            }
        }        
    }

    //--------------------------------------------------------------------------------------
    IResource * WorldResource::FindSceneResource(core::IBaseScene * _scene, core::BaseSceneType _sceneType)
    {
        WorldResourceData * worldResData = dynamic_cast<WorldResourceData *>(getObject());
        if (worldResData)
        {
            auto * sceneResources = worldResData->getScenes(_sceneType);

            for (uint i = 0; i < sceneResources->size(); ++i)
            {
                if ((*sceneResources)[i]->getObject() == _scene)
                    return (*sceneResources)[i];
            }
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    //void WorldResource::UnloadSceneResource(core::IResource * _resource, core::BaseSceneType _sceneType)
    //{
    //    WorldResourceData * worldResData = dynamic_cast<WorldResourceData *>(getObject());
    //    if (worldResData)
    //        VG_SAFE_RELEASE(_resource);
    //}

    //--------------------------------------------------------------------------------------
    void WorldResource::RemoveSceneResource(core::IResource * _resource, core::BaseSceneType _sceneType)
    {
        WorldResourceData * worldResData = dynamic_cast<WorldResourceData *>(getObject());
        if (worldResData)
        {
            worldResData->RemoveScene(VG_SAFE_STATIC_CAST(BaseSceneResource,_resource), _sceneType);
            VG_SAFE_RELEASE(_resource);
        }
    }

    //--------------------------------------------------------------------------------------
    core::uint WorldResource::GetSceneResourceCount(BaseSceneType _sceneType) const
    {
        WorldResourceData * worldResData = dynamic_cast<WorldResourceData *>(getObject());
        if (worldResData)
        {
            auto * sceneResources = worldResData->getScenes(_sceneType);
            return (core::uint)sceneResources->size();
        }

        return 0;
    }

    //--------------------------------------------------------------------------------------
    core::IResource * WorldResource::GetSceneResource(core::uint _index, BaseSceneType _sceneType) const
    {
        IObject * obj = getObject();
        VG_ASSERT(nullptr == obj || dynamic_cast<WorldResourceData*>(obj));
        WorldResourceData * worldResData = (WorldResourceData *)obj;
        if (worldResData)
        {
            auto * sceneResources = worldResData->getScenes(_sceneType);

            if (_index < sceneResources->size())
            {
                IResource * res = (*sceneResources)[_index];
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

        for (uint j = 0; j < enumCount<BaseSceneType>(); ++j)
        {
            auto sceneType = (BaseSceneType)j;
            auto * sceneResources = worldResData->getScenes(sceneType);

            for (uint i = 0; i < sceneResources->size(); ++i)
            {
                if (_resource == (*sceneResources)[i])
                {
                    IBaseScene * scene = dynamic_cast<IBaseScene *>(_resource->getObject());
                    if (scene)
                    {
                        if (worldResData->m_world)
                        {
                            switch (sceneType)
                            {
                            default:
                                VG_ASSERT_ENUM_NOT_IMPLEMENTED(sceneType);
                                break;

                                case BaseSceneType::Prefab:
                                if (worldResData->m_world->AddScene(scene, BaseSceneType::Prefab))
                                {
                                    //if (Engine::get()->isPlaying())
                                    //{
                                    //    IObject * root = scene->GetRoot();
                                    //    if (nullptr != root)
                                    //        root->OnPlay();
                                    //}
                                }
                                break;

                                case BaseSceneType::Scene:
                                if (worldResData->m_world->AddScene(scene, BaseSceneType::Scene))
                                {
                                    if (worldResData->m_world->IsPlaying())
                                    {
                                        IObject * root = scene->GetRoot();
                                        if (nullptr != root)
                                            root->OnPlay();
                                    }
                                }
                                break;
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

        for (uint j = 0; j < enumCount<BaseSceneType>(); ++j)
        {
            auto sceneType = (BaseSceneType)j;
            auto * sceneResources = worldResData->getScenes(sceneType);

            for (uint i = 0; i < sceneResources->size(); ++i)
            {
                if (_resource == (*sceneResources)[i])
                {
                    IBaseScene * scene = dynamic_cast<IBaseScene *>(_resource->getObject());
                    BaseSceneResource * sceneRes = VG_SAFE_STATIC_CAST(BaseSceneResource, _resource);
                    
                    if (worldResData->m_world)
                    {
                        switch (sceneType)
                        {
                            default:
                                VG_ASSERT_ENUM_NOT_IMPLEMENTED(sceneType);
                                break;

                            case BaseSceneType::Scene:
                                worldResData->m_world->RemoveScene(scene, sceneType);
                                //worldResData->RemoveScene(sceneRes, sceneType);
                                break;

                            case BaseSceneType::Prefab:
                                worldResData->m_world->RemoveScene(scene, sceneType);
                                //worldResData->RemoveScene(sceneRes, sceneType);
                                break;
                        }
                    }
                }
            }
        }
    }
}