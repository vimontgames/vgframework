#pragma once
#include "core/Object/Object.h"

namespace vg::engine
{
    class BaseSceneResource;
    class SceneResource;
    class PrefabResource;

    //--------------------------------------------------------------------------------------
    class WorldResourceData : public core::Object
    {
    public:
        VG_CLASS_DECL(WorldResourceData, core::Object);
        WorldResourceData(const core::string & _name = "", IObject * _parent = nullptr);
        ~WorldResourceData();

        void                                        createWorld();
        core::IWorld *                              getWorld() const { return m_world; }
        core::vector<BaseSceneResource *> *         getScenes(core::BaseSceneType _sceneType);
        void                                        OnPropertyChanged(core::IObject * _object, const core::IProperty & _prop, bool _notifyParent) override;

    //private:
        core::IWorld *                              m_world = nullptr;
        core::vector<SceneResource*>                m_sceneResources;
        core::vector<PrefabResource*>               m_prefabsResources;
    };
}