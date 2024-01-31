#pragma once

#include "core/IWorld.h"
#include "core/IBaseScene.h"

namespace vg::engine
{
    class Scene;

    //--------------------------------------------------------------------------------------
    // A "World" is a container for a list of scenes
    //--------------------------------------------------------------------------------------
    class World : public core::IWorld
    {
    public:
        VG_CLASS_DECL(World, core::IWorld);

        World(const core::string & _name, core::IObject * _parent);
        ~World();

        bool                    SetActiveScene  (core::IBaseScene * _scene, core::SceneType _sceneType) final override;
        core::IBaseScene *          GetActiveScene  (core::SceneType _sceneType) const final override;

        bool                    AddScene        (core::IBaseScene * _scene, core::SceneType _sceneType) final override;
        bool                    RemoveScene     (core::IBaseScene * scene, core::SceneType _sceneType) final override;
        core::uint              RemoveAllScenes (core::SceneType _sceneType) final override;

        core::uint              GetSceneCount   (core::SceneType _sceneType) const final override;
        const core::IBaseScene *    GetScene        (core::uint _index, core::SceneType _sceneType) const final override;

        core::vector<Scene*> &  getScenes       (core::SceneType _sceneType);

    private:
        Scene *                 m_activeScene[core::enumCount<core::SceneType>()];
        core::vector<Scene*>    m_scenes[core::enumCount<core::SceneType>()];
    };
}