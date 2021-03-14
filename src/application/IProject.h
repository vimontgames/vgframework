#pragma once

#include "core/IPlugin.h"

namespace vg
{
    namespace engine
    {
        class IEngine;
    }

    class IProject : public core::IPlugin
    {
    public:
                        IProject    () = default;
        virtual         ~IProject   () = default;

        virtual bool    init        (engine::IEngine & _engine) = 0;
        virtual bool    deinit      () = 0;

        virtual bool    update      () = 0;
    };
}