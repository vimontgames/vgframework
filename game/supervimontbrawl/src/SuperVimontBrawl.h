#pragma once

namespace vg::engine
{
    class IEngine;
}

class PlayerEntity;

class SuperVimontBrawlGame
{
    public:
        SuperVimontBrawlGame(vg::engine::IEngine & _engine);
        ~SuperVimontBrawlGame();

        bool update();

    private:
        vg::engine::IEngine &   m_engine;
        PlayerEntity *               m_player = nullptr;
};