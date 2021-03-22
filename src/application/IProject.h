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
                        IProject    () = default;
        virtual         ~IProject   () = default;

        virtual bool    init        (engine::IEngine & _engine, core::Singletons & _singletons) = 0;
        virtual bool    deinit      () = 0;

        virtual bool    update      () = 0;
    };
}