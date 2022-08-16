#pragma once

namespace vg::engine
{
    class IEngine;
}

class Player;

namespace vg
{
    class Application 
    {
    public:
        Application(vg::engine::IEngine & _engine);
        ~Application();

        bool update();

    private:
        vg::engine::IEngine & m_engine;
    };

}