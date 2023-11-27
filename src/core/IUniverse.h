#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // An universe is a container for N scenes
    //--------------------------------------------------------------------------------------
    class IScene;

    class IUniverse : public Object
    {
    public:
        IUniverse(const string & _name, IObject * _parent) :
            Object(_name, _parent)
        {

        }

        virtual ~IUniverse() {}

        virtual bool            SetActiveScene  (IScene * _scene) = 0;
        virtual core::IScene *  GetActiveScene  () const = 0;

        virtual bool            AddScene        (IScene * _scene) = 0;
        virtual bool            RemoveScene     (core::IScene * scene) = 0;
        virtual core::uint      RemoveAllScenes () = 0;

        virtual uint            GetSceneCount   () const = 0;
        virtual const IScene *  GetScene        (uint _index) const = 0;      
    };
}