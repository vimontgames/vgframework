#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // A "World" is a container for a list of scenes
    //--------------------------------------------------------------------------------------
    class IBaseScene;
    enum BaseSceneType : core::u8;

    class IWorld : public Object
    {
    public:
        IWorld(const string & _name, IObject * _parent) :
            Object(_name, _parent)
        {

        }

        virtual ~IWorld() {}

        virtual bool            SetActiveScene  (IBaseScene * _scene, BaseSceneType _sceneType) = 0;
        virtual core::IBaseScene *  GetActiveScene  (BaseSceneType _sceneType) const = 0;

        virtual bool            AddScene        (IBaseScene * _scene, BaseSceneType _sceneType) = 0;
        virtual bool            RemoveScene     (core::IBaseScene * scene, BaseSceneType _sceneType) = 0;
        virtual core::uint      RemoveAllScenes (BaseSceneType _sceneType) = 0;

        virtual uint            GetSceneCount   (BaseSceneType _sceneType) const = 0;
        virtual const IBaseScene *  GetScene        (uint _index, BaseSceneType _sceneType) const = 0;
    };
}