#pragma once

#include "core/IWorld.h"

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

        bool                    SetActiveScene(core::IScene * _scene) final override;
        core::IScene *          GetActiveScene() const final override;

        bool                    AddScene(core::IScene * _scene) final override;
        bool                    RemoveScene(core::IScene * scene) final override;
        core::uint              RemoveAllScenes() final override;

        core::uint              GetSceneCount() const final override;
        const core::IScene *    GetScene(core::uint _index) const final override;

        core::vector<Scene*> &  getScenes();

    private:
        Scene *                 m_activeScene = nullptr;
        core::vector<Scene*>    m_scenes;
    };
}