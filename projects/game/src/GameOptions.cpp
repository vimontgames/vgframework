#include "Precomp.h"
#include "GameOptions.h"

using namespace vg::core;

VG_REGISTER_CLASS(GameOptions, "Game Options");

//--------------------------------------------------------------------------------------
bool GameOptions::registerProperties(IClassDesc & _desc)
{
    super::registerProperties(_desc);

    registerProperty(GameOptions, m_deathHeight, "Death Height");
    setPropertyDescription(GameOptions, m_deathHeight, "Characters below this height will be considered dead");

    registerProperty(GameOptions, m_respawnHeight, "Respawn Height");
    setPropertyDescription(GameOptions, m_respawnHeight, "Characters will respawn at this height");

    return true;
}

//--------------------------------------------------------------------------------------
GameOptions::GameOptions(const string & _name, IObject * _parent) :
    super(_name, _parent)
{
    SetFile("Game.xml");
    Load(false);
}  

//--------------------------------------------------------------------------------------
GameOptions::~GameOptions()
{

}
