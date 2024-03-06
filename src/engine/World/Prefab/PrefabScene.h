#pragma once

#include "engine/World/GameObjectHierarchy.h"

namespace vg::engine
{
    class PrefabScene : public GameObjectHierarchy
    {
    public:
        VG_CLASS_DECL(PrefabScene, GameObjectHierarchy);

        PrefabScene(const core::string & _name, core::IObject * _parent);
        ~PrefabScene();

        core::BaseSceneType   GetSceneType() const final override { return core::BaseSceneType::Prefab; }
    };
}