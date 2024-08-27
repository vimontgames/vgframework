#include "WorldResourceData.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS(WorldResourceData, "World Resource Data");

    //--------------------------------------------------------------------------------------
    bool WorldResourceData::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyObjectPtr(WorldResourceData, m_world, "World");
        registerPropertyResourcePtrVector(WorldResourceData, m_sceneResources, "Scenes");
        registerPropertyResourcePtrVector(WorldResourceData, m_prefabsResources, "Prefabs");

        return true;
    }

    //--------------------------------------------------------------------------------------
    WorldResourceData::WorldResourceData(const core::string & _name, IObject * _parent) :
        super(_name, _parent)
    {
        
    }

    //--------------------------------------------------------------------------------------
    WorldResourceData::~WorldResourceData()
    {
        // The copy here is intentional, because release scene resources will remove them from the container
        for (uint j = 0; j < enumCount<BaseSceneType>(); ++j)
        {
            auto sceneType = (BaseSceneType)j;
            auto scenesToRelease = *getScenes(sceneType);

            for (uint i = 0; i < scenesToRelease.size(); ++i)
                VG_SAFE_RELEASE(scenesToRelease[i]);
            getScenes(sceneType)->clear();
        }

        VG_SAFE_RELEASE(m_world);
    }

    //--------------------------------------------------------------------------------------
    void WorldResourceData::createWorld()
    {
        VG_SAFE_RELEASE(m_world);
        IFactory * factory = Kernel::getFactory();
        m_world = (IWorld *)factory->createObject("World");
        m_world->SetParent((IObject *)this);
    }

    //--------------------------------------------------------------------------------------
    void WorldResourceData::OnPropertyChanged(IObject * _object, const IProperty & _prop, bool _notifyParent)
    {
        // [...]

        super::OnPropertyChanged(_object, _prop, _notifyParent);
    }

    //--------------------------------------------------------------------------------------
    core::vector<BaseSceneResource *> * WorldResourceData::getScenes(core::BaseSceneType _sceneType)
    {
        switch (_sceneType)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_sceneType)
                return nullptr;

            case BaseSceneType::Scene:
                return (core::vector<BaseSceneResource *> *) &m_sceneResources;
    
            case BaseSceneType::Prefab:
                return (core::vector<BaseSceneResource *> *) &m_prefabsResources;
        }
    }

    //--------------------------------------------------------------------------------------
    void WorldResourceData::RemoveScene(BaseSceneResource * _sceneRes, BaseSceneType _sceneType)
    {
        IBaseScene * scene = dynamic_cast<IBaseScene *>(_sceneRes->GetObject());
        switch (_sceneType)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_sceneType);
                break;

            case core::Scene:
                m_sceneResources.remove((SceneResource*)_sceneRes);
                break;

            case core::Prefab:
                m_prefabsResources.remove((PrefabResource *)_sceneRes);
                break;
        }
        m_world->RemoveScene(scene, _sceneType);
    }
}