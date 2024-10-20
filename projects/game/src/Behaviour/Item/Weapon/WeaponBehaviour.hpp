#include "WeaponBehaviour.h"
#include "Game.h"
#include "Behaviour/Character/Player/PlayerBehaviour.h"
#include "engine/IAttachToNodeComponent.h"
#include "engine/IEngineOptions.h"

using namespace vg::core;
using namespace vg::engine;

VG_REGISTER_COMPONENT_CLASS(WeaponBehaviour, "Weapon", "Game", "A weapon the player can pick", vg::editor::style::icon::Script, 0);

//--------------------------------------------------------------------------------------
WeaponBehaviour::WeaponBehaviour(const string & _name, IObject * _parent) :
    super(_name, _parent, ItemType::Weapon)
{
    EnableUpdateFlags(vg::core::UpdateFlags::Update, false);
}

//--------------------------------------------------------------------------------------
WeaponBehaviour::~WeaponBehaviour()
{

}

//--------------------------------------------------------------------------------------
bool WeaponBehaviour::registerProperties(IClassDesc & _desc)
{
    super::registerProperties(_desc);

    registerPropertyEnum(WeaponBehaviour, WeaponType, m_weaponType, "Type");
    setPropertyDescription(WeaponBehaviour, m_weaponType, "Weapon type");

    return true;
}

//--------------------------------------------------------------------------------------
void WeaponBehaviour::OnPlay()
{
    super::OnPlay();
    m_ennemyTag = Game::get()->Engine().GetOptions()->GetGameObjectTag("Enemy");
}

//--------------------------------------------------------------------------------------
void WeaponBehaviour::OnTriggerEnter(vg::core::IGameObject * _other)
{
    if (asBool(m_ennemyTag & _other->GetTags()))
    {
        if (auto * owner = GetOwner().get<IGameObject>())
        {
            if (auto * attacker = owner->GetComponentT<CharacterBehaviour>())
            {
                if (attacker->getFightState() == FightState::Hit)
                {
                    if (auto * defender = _other->GetComponentT<CharacterBehaviour>())
                        defender->takeHit(attacker, this);
                }
            }
        }
    }
}

//--------------------------------------------------------------------------------------
void WeaponBehaviour::OnCollisionEnter(vg::core::IGameObject * _other)
{
    // Weapons do not set owner on touch but only when picked
   //super::OnCollisionEnter(_object);
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


