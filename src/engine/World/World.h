#pragma once

#include "core/IWorld.h"
#include "core/IBaseScene.h"

namespace vg::physics
{
    class IBody;
}

namespace vg::engine
{
    class Scene;

    //--------------------------------------------------------------------------------------
    // A "World" is a container for a list of scenes
    //--------------------------------------------------------------------------------------
    class World final : public core::IWorld
    {
    public:
        VG_CLASS_DECL(World, core::IWorld);

        World(const core::string & _name, core::IObject * _parent);
        ~World();

        bool                                IsPlaying                       () const final override;
        bool                                IsPaused                        () const final override;

        void                                OnPlay                          () final override;
        void                                OnStop                          () final override;
        void                                OnPause                         () final override;
        void                                OnResume                        () final override;

        void                                BeforeUpdate                    (const Context & _context) final override;
        void                                AfterUpdate                     (const Context & _context) final override;

        bool                                SetActiveScene                  (core::IBaseScene * _scene, core::BaseSceneType _sceneType) final override;
        core::IBaseScene *                  GetActiveScene                  (core::BaseSceneType _sceneType) const final override;

        bool                                AddScene                        (core::IBaseScene * _scene, core::BaseSceneType _sceneType) final override;
        bool                                RemoveScene                     (core::IBaseScene * scene, core::BaseSceneType _sceneType) final override;
        core::uint                          RemoveAllScenes                 (core::BaseSceneType _sceneType) final override;

        core::uint                          GetSceneCount                   (core::BaseSceneType _sceneType) const final override;
        const core::IBaseScene *            GetScene                        (core::uint _index, core::BaseSceneType _sceneType) const final override;

        core::vector<core::IBaseScene *> &  getScenes                       (core::BaseSceneType _sceneType);

        void                                SetDebugDrawData                (core::IDebugDrawData * _debugDrawData) final override;
        core::IDebugDrawData *              GetDebugDrawData                () const final override;

        void                                SetPhysicsWorld                 (core::IPhysicsWorld * _physicsWorld) final override;
        core::IPhysicsWorld *               GetPhysicsWorld                 () const final override;

        bool                                IsPrefabWorld                   () const final override;

        void                                SetEnvironmentColor             (const core::float4 & _environmentColor) final override;
        core::float4                        GetEnvironmentColor             () const final override;

        void                                SetEnvironmentCubemap           (gfx::ITexture * _texture) final override;
        gfx::ITexture *                     GetEnvironmentCubemap           () const final override;

        void                                SetEnvironmentAmbientIntensity  (float _ambientIntensity) final override;
        float                               GetEnvironmentAmbientIntensity  () const final override;

    protected:
        void                                mergeStaticBodies               ();
        void                                clearUI                         ();

    private:
        core::IBaseScene *                  m_activeScene[core::enumCount<core::BaseSceneType>()];
        core::vector<core::IBaseScene *>    m_scenes[core::enumCount<core::BaseSceneType>()];
        core::IDebugDrawData *              m_debugDrawData = nullptr;
        core::IPhysicsWorld *               m_physicsWorld = nullptr;
        core::vector<physics::IBody *>      m_staticColliders;

        core::float4                        m_currentEnvironmentColor = (core::float4)0.0f;
        core::float4                        m_nextEnvironmentColor = (core::float4)0.0f;
        gfx::ITexture *                     m_defaultEnvironmentCubemap = nullptr;
        gfx::ITexture *                     m_environmentCubemap = nullptr;
        float                               m_environmentAmbientIntensity = 0.0f;
    };
}

#if VG_ENABLE_INLINE
#include "World.inl"
#endif