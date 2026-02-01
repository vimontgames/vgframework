#pragma once

#include "core/Object/Object.h"
#include "core/Object/Update.h"

// rem This is not good to include gfx interface here, have to find another way
namespace vg::gfx
{
    class ITexture;
}

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // A "World" is a container for a list of scenes
    //--------------------------------------------------------------------------------------
    class IBaseScene;
    enum BaseSceneType : core::u8;

    class IDebugDrawData
    {
    public:
        virtual ~IDebugDrawData() {}
    };

    class IPhysicsWorld : public core::Object
    {
    public:
        virtual ~IPhysicsWorld() {}
        virtual void DrawDebug() = 0;
    };

    class IWorld : public Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IWorld, Object);

        virtual ~IWorld() {}

        virtual bool                IsPlaying                       () const = 0;
        virtual bool                IsPaused                        () const = 0;

        virtual void                OnPlay                          () = 0;
        virtual void                OnStop                          () = 0;
        virtual void                OnPause                         () = 0;
        virtual void                OnResume                        () = 0;

        virtual bool                SetActiveScene                  (IBaseScene * _scene, BaseSceneType _sceneType) = 0;
        virtual core::IBaseScene *  GetActiveScene                  (BaseSceneType _sceneType) const = 0;

        virtual bool                AddScene                        (IBaseScene * _scene, BaseSceneType _sceneType) = 0;
        virtual bool                RemoveScene                     (core::IBaseScene * scene, BaseSceneType _sceneType) = 0;
        virtual core::uint          RemoveAllScenes                 (BaseSceneType _sceneType) = 0;

        virtual uint                GetSceneCount                   (BaseSceneType _sceneType) const = 0;
        virtual const IBaseScene *  GetScene                        (uint _index, BaseSceneType _sceneType) const = 0;

        virtual void                SetDebugDrawData                (IDebugDrawData * _debugDrawData) = 0;
        virtual IDebugDrawData *    GetDebugDrawData                () const = 0;

        virtual void                SetPhysicsWorld                 (IPhysicsWorld * _physicsWorld) = 0;
        virtual IPhysicsWorld *     GetPhysicsWorld                 () const = 0;

        virtual bool                IsPrefabWorld                   () const = 0;

        virtual void                SetEnvironmentColor             (const core::float4 & _environmentColor) = 0;
        virtual core::float4        GetEnvironmentColor             () const = 0;

        virtual void                SetEnvironmentCubemap           (gfx::ITexture * _texture) = 0;
        virtual gfx::ITexture *     GetEnvironmentCubemap           () const = 0;

        virtual void                SetIrradianceCubemap            (gfx::ITexture * _texture) = 0;
        virtual gfx::ITexture *     GetIrradianceCubemap            () const = 0;

        virtual void                SetSpecularReflectionCubemap    (gfx::ITexture * _texture) = 0;
        virtual gfx::ITexture *     GetSpecularReflectionCubemap    () const = 0;

        virtual void                SetIrradianceIntensity          (float _value) = 0;
        virtual float               GetIrradianceIntensity          () const = 0;
         
        virtual void                SetSpecularReflectionIntensity  (float _value) = 0;
        virtual float               GetSpecularReflectionIntensity  () const = 0;
    };
}