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

        virtual bool            setActiveScene  (IScene * _scene) = 0;
        virtual core::IScene *  getActiveScene  () const = 0;

        virtual bool            addScene        (IScene * _scene) = 0;
        virtual bool            removeScene     (core::IScene * scene) = 0;
        virtual core::uint      removeAllScenes () = 0;

        virtual uint            getSceneCount   () const = 0;
        virtual const IScene *  getScene        (uint _index) const = 0;      
    };
}