#pragma once

namespace vg::engine
{
    class IEngine;
}

class SuperVimontBrawlGame
{
    public:
        SuperVimontBrawlGame(vg::engine::IEngine * _engine);
        ~SuperVimontBrawlGame();

    private:
        vg::engine::IEngine & m_engine;
};