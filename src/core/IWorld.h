#pragma once

#include "core/Object/Object.h"

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
    };

    class IWorld : public Object
    {
    public:
        IWorld(const string & _name, IObject * _parent) :
            Object(_name, _parent)
        {

        }

        virtual ~IWorld() {}

        virtual bool                IsPlaying               () const = 0;
        virtual bool                IsPaused                () const = 0;

        virtual void                OnPlay                  () = 0;
        virtual void                OnStop                  () = 0;
        virtual void                OnPause                 () = 0;
        virtual void                OnResume                () = 0;

        virtual bool                SetActiveScene          (IBaseScene * _scene, BaseSceneType _sceneType) = 0;
        virtual core::IBaseScene *  GetActiveScene          (BaseSceneType _sceneType) const = 0;

        virtual bool                AddScene                (IBaseScene * _scene, BaseSceneType _sceneType) = 0;
        virtual bool                RemoveScene             (core::IBaseScene * scene, BaseSceneType _sceneType) = 0;
        virtual core::uint          RemoveAllScenes         (BaseSceneType _sceneType) = 0;

        virtual uint                GetSceneCount           (BaseSceneType _sceneType) const = 0;
        virtual const IBaseScene *  GetScene                (uint _index, BaseSceneType _sceneType) const = 0;

        virtual void                SetDebugDrawData        (IDebugDrawData * _debugDrawData) = 0;
        virtual IDebugDrawData *    GetDebugDrawData        () const = 0;

        virtual void                SetPhysicsWorld         (IPhysicsWorld * _physicsWorld) = 0;
        virtual IPhysicsWorld *     GetPhysicsWorld         () const = 0;

        virtual bool                IsPrefabWorld           () const = 0;
    };
}