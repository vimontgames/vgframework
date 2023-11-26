#include "Precomp.h"
#include "PlayerBehaviour.h"

using namespace vg::core;

VG_REGISTER_COMPONENT_CLASS(PlayerBehaviour, "PlayerBehaviour Component");

//--------------------------------------------------------------------------------------
PlayerBehaviour::PlayerBehaviour(const string & _name, IObject * _parent) :
    Component(_name, nullptr)
{

}

//--------------------------------------------------------------------------------------
PlayerBehaviour::~PlayerBehaviour()
{

}

//--------------------------------------------------------------------------------------
bool PlayerBehaviour::registerProperties(IClassDesc & _desc)
{
    super::registerProperties(_desc);

    _desc.registerProperty(PlayerBehaviour, m_MyFloat, "My Super Float");
    
    return true;
}

//--------------------------------------------------------------------------------------
void PlayerBehaviour::Update(double _dt)
{

}