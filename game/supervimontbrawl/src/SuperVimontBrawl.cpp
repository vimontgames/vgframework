#include "Precomp.h"
#include "SuperVimontBrawl.h"
#include "engine/IEngine.h"
#include "Entity/Player/PlayerEntity.h"

//--------------------------------------------------------------------------------------
SuperVimontBrawlGame::SuperVimontBrawlGame(vg::engine::IEngine & _engine) :
    m_engine(_engine)
{
    const auto screenSize = _engine.getScreenSize();
    m_player = new PlayerEntity("Roméo");
}

//--------------------------------------------------------------------------------------
SuperVimontBrawlGame::~SuperVimontBrawlGame()
{
    VG_SAFE_RELEASE(m_player);
}

//--------------------------------------------------------------------------------------
bool SuperVimontBrawlGame::update()
{
    m_engine.runOneFrame();

    return true;
}