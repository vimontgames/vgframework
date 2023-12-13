#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // A "World" is a container for a list of scenes
    //--------------------------------------------------------------------------------------
    class IScene;

    class IWorld : public Object
    {
    public:
        IWorld(const string & _name, IObject * _parent) :
            Object(_name, _parent)
        {

        }

        virtual ~IWorld() {}

        virtual bool            SetActiveScene  (IScene * _scene) = 0;
        virtual core::IScene *  GetActiveScene  () const = 0;

        virtual bool            AddScene        (IScene * _scene) = 0;
        virtual bool            RemoveScene     (core::IScene * scene) = 0;
        virtual core::uint      RemoveAllScenes () = 0;

        virtual uint            GetSceneCount   () const = 0;
        virtual const IScene *  GetScene        (uint _index) const = 0;      
    };
}