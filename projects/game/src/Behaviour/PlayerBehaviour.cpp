#include "Precomp.h"
#include "PlayerBehaviour.h"

using namespace vg::core;

VG_AUTO_REGISTER_CLASS(PlayerBehaviour);

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
bool PlayerBehaviour::registerClass(IFactory & _factory)
{
    if (IClassDesc * desc = _factory.registerClassHelper(PlayerBehaviour, "Player Component", IClassDesc::Flags::Component))
        registerProperties(*desc);

    return true;
}

//--------------------------------------------------------------------------------------
bool PlayerBehaviour::registerProperties(IClassDesc & _desc)
{
    super::registerProperties(_desc);

    _desc.registerPropertyHelper(PlayerBehaviour, m_MyFloat, "My Super Float");
    
    return true;
}

//--------------------------------------------------------------------------------------
void PlayerBehaviour::Update(double _dt)
{

}