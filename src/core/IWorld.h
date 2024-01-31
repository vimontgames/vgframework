#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // A "World" is a container for a list of scenes
    //--------------------------------------------------------------------------------------
    class IBaseScene;
    enum SceneType : core::u8;

    class IWorld : public Object
    {
    public:
        IWorld(const string & _name, IObject * _parent) :
            Object(_name, _parent)
        {

        }

        virtual ~IWorld() {}

        virtual bool            SetActiveScene  (IBaseScene * _scene, SceneType _sceneType) = 0;
        virtual core::IBaseScene *  GetActiveScene  (SceneType _sceneType) const = 0;

        virtual bool            AddScene        (IBaseScene * _scene, SceneType _sceneType) = 0;
        virtual bool            RemoveScene     (core::IBaseScene * scene, SceneType _sceneType) = 0;
        virtual core::uint      RemoveAllScenes (SceneType _sceneType) = 0;

        virtual uint            GetSceneCount   (SceneType _sceneType) const = 0;
        virtual const IBaseScene *  GetScene        (uint _index, SceneType _sceneType) const = 0;
    };
}