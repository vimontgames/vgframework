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

        bool                                IsPlaying               () const final override;
        bool                                IsPaused                () const final override;
        void                                SetTimeScale            (float _timeScale) final override;
        float                               GetTimeScale            (bool _ignorePause = false) const final override;
        void                                Play                    () final override;
        void                                Stop                    () final override;
        void                                Pause                   () final override;
        void                                Resume                  () final override;

        bool                                IsUsingFixedDeltaTime   () const final override;
        void                                SetUseFixedDeltaTime    (bool _fixedDeltaTime) final override;

        bool                                SetActiveScene          (core::IBaseScene * _scene, core::BaseSceneType _sceneType) final override;
        core::IBaseScene *                  GetActiveScene          (core::BaseSceneType _sceneType) const final override;

        bool                                AddScene                (core::IBaseScene * _scene, core::BaseSceneType _sceneType) final override;
        bool                                RemoveScene             (core::IBaseScene * scene, core::BaseSceneType _sceneType) final override;
        core::uint                          RemoveAllScenes         (core::BaseSceneType _sceneType) final override;

        core::uint                          GetSceneCount           (core::BaseSceneType _sceneType) const final override;
        const core::IBaseScene *            GetScene                (core::uint _index, core::BaseSceneType _sceneType) const final override;

        core::vector<core::IBaseScene *> &  getScenes               (core::BaseSceneType _sceneType);

        void                                SetDebugDrawData        (core::IDebugDrawData * _debugDrawData) final override;
        core::IDebugDrawData *              GetDebugDrawData        () const final override;

        void                                SetPhysicsWorld         (core::IPhysicsWorld * _physicsWorld) final override;
        core::IPhysicsWorld *               GetPhysicsWorld         () const final override;

        bool                                IsPrefabWorld           () const final override;

    protected:
        VG_INLINE bool                      isPlaying               () const;
        VG_INLINE void                      setTimeScale            (float _timeScale);
        VG_INLINE float                     getTimeScale            (bool _ignorePause = false) const;
        void                                play                    ();
        void                                stop                    ();

        VG_INLINE bool                      isPaused                () const;
        void                                pause                   ();
        void                                resume                  ();

    private:
        bool                                m_isPlaying     : 1;
        bool                                m_isPaused      : 1;
        bool                                m_isFixedDT     : 1;
        float                               m_timeScale;
        core::IBaseScene *                  m_activeScene[core::enumCount<core::BaseSceneType>()];
        core::vector<core::IBaseScene *>    m_scenes[core::enumCount<core::BaseSceneType>()];
        core::IDebugDrawData *              m_debugDrawData = nullptr;
        core::IPhysicsWorld *               m_physicsWorld = nullptr;
    };
}

#if VG_ENABLE_INLINE
#include "World.inl"
#endif