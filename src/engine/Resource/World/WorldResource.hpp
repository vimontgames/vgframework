#include "WorldResource.h"
#include "core/File/File.h"
#include "core/IWorld.h"
#include "engine/Resource/World/WorldResourceData.hpp"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_RESOURCE_CLASS(WorldResource, "World", -1000);

    //--------------------------------------------------------------------------------------
    void * ResizeWorldResourceVector(IObject * _parent, uint _offset, uint _count)
    {
        auto vec = (core::vector<WorldResource> *)(uint_ptr(_parent) + _offset);
        vec->clear();
        vec->resize(_count);
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
       
    }

    //--------------------------------------------------------------------------------------
    const core::vector<core::string> WorldResource::GetExtensions() const
    {
        vector<string> ext;
        ext.push_back(".world");
        return ext;
    }

    //--------------------------------------------------------------------------------------
    bool WorldResource::Cook(const string & _file) const
    {
        return super::Cook(_file);
    }

    //--------------------------------------------------------------------------------------
    LoadStatus WorldResource::Load(const core::string & _path, core::IObject *& _object)
    {
        IFactory * factory = Kernel::getFactory();
        WorldResourceData * worldData = dynamic_cast<WorldResourceData *>(factory->CreateObject("WorldResourceData"));
        if (nullptr != worldData)
        {
            worldData->SetParent(this);
            if (factory->LoadFromXML(worldData, _path))
            {
                worldData->createWorld();

                _object = worldData;
                return LoadStatus::Success;
            }
            else
            {
                VG_SAFE_RELEASE(worldData);
                return LoadStatus::CannotOpenFile;                
            }
        }

        VG_SAFE_RELEASE(worldData);
        return LoadStatus::CannotCreateObject;
    }

    //--------------------------------------------------------------------------------------
    // a .scene file is an XML file that serializes a Scene Object
    //--------------------------------------------------------------------------------------
    bool WorldResource::CreateFile(const core::string & _path, core::IObject * _data)
    {
        const auto * factory = Kernel::getFactory();

        WorldResourceData * worldData = dynamic_cast<WorldResourceData *>(factory->CreateObject("WorldResourceData", core::io::getFileNameWithoutExt(_path)));
        if (nullptr != worldData)
        {
            factory->SaveToXML(worldData, _path);
            VG_SAFE_RELEASE(worldData);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool WorldResource::SaveFile(const string & _path) const
    {
        WorldResourceData * worldData = dynamic_cast<WorldResourceData *>(GetObject());
        if (nullptr != worldData)
        {
            const auto * factory = Kernel::getFactory();
            return factory->SaveToXML(worldData, _path);
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    core::IWorld * WorldResource::GetWorld() const
    {
        WorldResourceData * worldResData = dynamic_cast<WorldResourceData *>(GetObject());
        if (worldResData)
            return dynamic_cast<IWorld *>(worldResData->getWorld());
        
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    void WorldResource::CreateSceneResource(const core::string & _file, core::BaseSceneType _sceneType)
    {
        WorldResourceData * worldResData = dynamic_cast<WorldResourceData *>(GetObject());
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
                    sceneRes->SetName(io::getFileNameWithoutExt(_file));

                    if (sceneRes->CreateFile(_file))
                    {
                        sceneRes->SetParent(this);
                        sceneRes->SetResourcePath(_file);
                        worldResData->m_sceneResources.push_back(sceneRes);
                    }
                }
                break;

                case core::Prefab:
                {
                    PrefabResource * prefabRes = new PrefabResource();
                    prefabRes->SetName(io::getFileNameWithoutExt(_file));

                    if (prefabRes->CreateFile(_file))
                    {
                        prefabRes->SetParent(this);
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
        WorldResourceData * worldResData = dynamic_cast<WorldResourceData *>(GetObject());
        if (worldResData)
        {
            switch (_sceneType)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(_sceneType);
                    break;

                case core::Scene:
                {
                    auto it = std::find_if(worldResData->m_sceneResources.begin(), worldResData->m_sceneResources.end(), 
                        [&_file](const SceneResource * _res) 
                        {
                            return 0 == _res->GetResourcePath().compare(io::getRelativePath(_file));
                        }
                    );

                    if (it == worldResData->m_sceneResources.end())
                    {
                        SceneResource * sceneRes = new SceneResource();
                        sceneRes->SetParent(worldResData);
                        sceneRes->SetResourcePath(_file);
                        worldResData->m_sceneResources.push_back(sceneRes);
                    }
                    else
                    {
                        VG_WARNING("[World] Scene \"%s\" is already loaded", _file.c_str());
                    }                    
                }
                break;

                case core::Prefab:
                {
                    auto it = std::find_if(worldResData->m_prefabsResources.begin(), worldResData->m_prefabsResources.end(),
                        [&_file](const PrefabResource * _res) 
                        {
                            return 0 == _res->GetResourcePath().compare(io::getRelativePath(_file));
                        }
                    );

                    if (it == worldResData->m_prefabsResources.end())
                    {
                        PrefabResource * prefabRes = new PrefabResource();
                        prefabRes->SetParent(worldResData);
                        prefabRes->SetResourcePath(_file);
                        worldResData->m_prefabsResources.push_back(prefabRes);
                    }
                    else
                    {
                        VG_WARNING("[World] Prefab \"%s\" is already loaded", _file.c_str());
                    }
                }
                break;
            }
        }        
    }

    //--------------------------------------------------------------------------------------
    IResource * WorldResource::FindSceneResource(core::IBaseScene * _scene, core::BaseSceneType _sceneType)
    {
        WorldResourceData * worldResData = dynamic_cast<WorldResourceData *>(GetObject());
        if (worldResData)
        {
            auto * sceneResources = worldResData->getScenes(_sceneType);

            for (uint i = 0; i < sceneResources->size(); ++i)
            {
                if ((*sceneResources)[i]->GetObject() == _scene)
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
        WorldResourceData * worldResData = dynamic_cast<WorldResourceData *>(GetObject());
        if (worldResData)
        {
            worldResData->RemoveScene(VG_SAFE_STATIC_CAST(BaseSceneResource,_resource), _sceneType);
            VG_SAFE_RELEASE(_resource);
        }
    }

    //--------------------------------------------------------------------------------------
    core::uint WorldResource::GetSceneResourceCount(BaseSceneType _sceneType) const
    {
        WorldResourceData * worldResData = dynamic_cast<WorldResourceData *>(GetObject());
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
        IObject * obj = GetObject();
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
    void WorldResource::OnResourceLoaded(core::IResource * _resource)
    {
        VG_PROFILE_CPU("WorldResource");

        WorldResourceData * worldResData = dynamic_cast<WorldResourceData *>(GetObject());
        VG_ASSERT(worldResData);

        for (uint j = 0; j < enumCount<BaseSceneType>(); ++j)
        {
            auto sceneType = (BaseSceneType)j;
            auto * sceneResources = worldResData->getScenes(sceneType);

            for (uint i = 0; i < sceneResources->size(); ++i)
            {
                if (_resource == (*sceneResources)[i])
                {
                    IBaseScene * scene = dynamic_cast<IBaseScene *>(_resource->GetObject());
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
    void WorldResource::OnResourceUnloaded(core::IResource * _resource)
    {
        WorldResourceData * worldResData = dynamic_cast<WorldResourceData *>(GetObject());
        VG_ASSERT(worldResData);

        for (uint j = 0; j < enumCount<BaseSceneType>(); ++j)
        {
            auto sceneType = (BaseSceneType)j;
            auto * sceneResources = worldResData->getScenes(sceneType);

            for (uint i = 0; i < sceneResources->size(); ++i)
            {
                if (_resource == (*sceneResources)[i])
                {
                    IBaseScene * scene = dynamic_cast<IBaseScene *>(_resource->GetObject());
                    
                    if (worldResData->m_world)
                    {
                        switch (sceneType)
                        {
                            default:
                                VG_ASSERT_ENUM_NOT_IMPLEMENTED(sceneType);
                                break;

                            case BaseSceneType::Scene:
                                worldResData->m_world->RemoveScene(scene, sceneType);
                                break;

                            case BaseSceneType::Prefab:
                                worldResData->m_world->RemoveScene(scene, sceneType);
                                break;
                        }
                    }
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    // As opposed to other resources types, WorldResource are parent of Prefabs and it is possible to override children properties (unlike e.g. materials)
    //--------------------------------------------------------------------------------------
    bool WorldResource::CanOverrideProperties() const
    {
        return true;
    }
}