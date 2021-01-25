#include "Precomp.h"
#include "SuperVimontBrawl.h"

//--------------------------------------------------------------------------------------
SuperVimontBrawlGame::SuperVimontBrawlGame(vg::engine::IEngine * _engine) :
    m_engine(*_engine)
{
    VG_ASSERT(_engine);
}

//--------------------------------------------------------------------------------------
SuperVimontBrawlGame::~SuperVimontBrawlGame()
{

}