#include "Precomp.h"
#include "GameOptions.h"

using namespace vg::core;

VG_REGISTER_OBJECT_CLASS(GameOptions, "Game Options");

//--------------------------------------------------------------------------------------
bool GameOptions::registerProperties(IClassDesc & _desc)
{
    super::registerProperties(_desc);

    registerProperty(GameOptions, m_minZ, "Minimum height");
    setPropertyDescription(GameOptions, m_minZ, "Characters below this height will be considered dead");

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
