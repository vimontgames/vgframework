#include "Precomp.h"
#include "PlayerComponent.h"

using namespace vg::core;

VG_AUTO_REGISTER_CLASS(PlayerComponent);

//--------------------------------------------------------------------------------------
PlayerComponent::PlayerComponent(const string & _name, IObject * _parent) :
    Component("Player", nullptr)
{

}

//--------------------------------------------------------------------------------------
PlayerComponent::~PlayerComponent()
{

}

//--------------------------------------------------------------------------------------
bool PlayerComponent::registerClass(IFactory & _factory)
{
    if (IClassDesc * desc = _factory.registerClassHelper(PlayerComponent, "Player Component", IClassDesc::Flags::Component))
        registerProperties(*desc);

    return true;
}

//--------------------------------------------------------------------------------------
bool PlayerComponent::registerProperties(IClassDesc & _desc)
{
    super::registerProperties(_desc);

    _desc.registerPropertyHelper(PlayerComponent, m_MyFloat, "My Super Float", IProperty::Flags::None);
    
    return true;
}



//--------------------------------------------------------------------------------------
void PlayerComponent::Update(double _dt)
{

}