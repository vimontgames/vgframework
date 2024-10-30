#pragma once

#include "core/IPlugin.h"

namespace vg::engine
{
    class IEngine;
}

namespace vg
{
    class Application 
    {
    public:
        Application(vg::engine::IEngine & _engine);
        ~Application();

        bool Update();

    private:
        vg::engine::IEngine & m_engine;
    };

}