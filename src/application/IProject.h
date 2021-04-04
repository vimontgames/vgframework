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

    class IProject : public core::IPlugin
    {
    public:
        IProject(const core::string & _name, core::IObject * _parent) :
            core::IPlugin(_name, _parent)
        {

        }

        virtual         ~IProject   () = default;

        virtual bool    init        (engine::IEngine & _engine, core::Singletons & _singletons) = 0;
        virtual bool    deinit      () = 0;

        virtual bool    update      () = 0;
    };
}