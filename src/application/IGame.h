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

    class IGame : public core::IPlugin
    {
    public:
        IGame(const core::string & _name, core::IObject * _parent) :
            core::IPlugin(_name, _parent)
        {

        }

        virtual         ~IGame   () = default;

        virtual bool    Init        (engine::IEngine & _engine, core::Singletons & _singletons) = 0;
        virtual bool    Deinit      () = 0;
    };
}