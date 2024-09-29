#include "WeaponBehaviour.h"
#include "editor/Editor_Consts.h"
#include "Game.h"
#include "Behaviour/Character/Player/PlayerBehaviour.h"
#include "engine/IAttachToNodeComponent.h"

using namespace vg::core;
using namespace vg::engine;

VG_REGISTER_COMPONENT_CLASS(WeaponBehaviour, "Weapon", "Game", "A weapon the player can pick", vg::editor::style::icon::Script, 0);

//--------------------------------------------------------------------------------------
WeaponBehaviour::WeaponBehaviour(const string & _name, IObject * _parent) :
    super(_name, _parent, ItemType::Weapon)
{

}

//--------------------------------------------------------------------------------------
WeaponBehaviour::~WeaponBehaviour()
{

}

//--------------------------------------------------------------------------------------
bool WeaponBehaviour::registerProperties(IClassDesc & _desc)
{
    super::registerProperties(_desc);

    return true;
}

//--------------------------------------------------------------------------------------
void WeaponBehaviour::OnPlay()
{
    super::OnPlay();
}

//--------------------------------------------------------------------------------------
void WeaponBehaviour::Update(const Context & _context)
{

}

//--------------------------------------------------------------------------------------
void WeaponBehaviour::SetOwner(vg::core::IGameObject * _object)
{
    super::SetOwner(_object);

    if (auto * attachToNode = GetGameObject()->GetComponentT<vg::engine::IAttachToNodeComponent>())
    {
        attachToNode->SetUseTarget(true);
        attachToNode->SetTarget(_object);
    }
}


