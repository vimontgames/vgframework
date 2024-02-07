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

        bool                        SetActiveScene      (core::IBaseScene * _scene, core::BaseSceneType _sceneType) final override;
        core::IBaseScene *          GetActiveScene      (core::BaseSceneType _sceneType) const final override;

        bool                        AddScene            (core::IBaseScene * _scene, core::BaseSceneType _sceneType) final override;
        bool                        RemoveScene         (core::IBaseScene * scene, core::BaseSceneType _sceneType) final override;
        core::uint                  RemoveAllScenes     (core::BaseSceneType _sceneType) final override;

        core::uint                  GetSceneCount       (core::BaseSceneType _sceneType) const final override;
        const core::IBaseScene *    GetScene            (core::uint _index, core::BaseSceneType _sceneType) const final override;

        core::vector<Scene*> &      getScenes           (core::BaseSceneType _sceneType);

        void                        SetDebugDrawData    (core::IDebugDrawData * _debugDrawData) final override;
        core::IDebugDrawData *      GetDebugDrawData    () const final override;

        void                        SetPhysicsWorld     (core::IPhysicsWorld * _physicsWorld) final override;
        core::IPhysicsWorld *       GetPhysicsWorld     () const final override;

    private:
        Scene *                     m_activeScene[core::enumCount<core::BaseSceneType>()];
        core::vector<Scene*>        m_scenes[core::enumCount<core::BaseSceneType>()];
        core::IDebugDrawData *      m_debugDrawData = nullptr;
        core::IPhysicsWorld *       m_physicsWorld = nullptr;
    };
}