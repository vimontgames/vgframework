#pragma once

#include "core/IPlugin.h"

namespace vg
{
    namespace core
    {
        struct Singletons;
    }

    namespace engine
    {
        class IEngine;
    }

    class IGameOptions;

    class IGame : public core::IPlugin
    {
    public:
        IGame(const core::string & _name, core::IObject * _parent) :
            core::IPlugin(_name, _parent)
        {

        }

        virtual                 ~IGame      () = default;

        virtual IGameOptions *  GetOptions  () const = 0;

        virtual bool            Init        (engine::IEngine & _engine, core::Singletons & _singletons) = 0;
        virtual bool            Deinit      () = 0;

        virtual void            OnPlay      () = 0;
        virtual void            OnStop      () = 0;
    };
}