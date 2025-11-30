#include "WeaponBehaviour.h"
#include "Game.h"
#include "Behaviour/Entity/Character/Player/PlayerBehaviour.h"
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
bool IsProjectilePropertyHidden(const IObject * _object, const IProperty * _prop, uint _index)
{
    const WeaponBehaviour * weapon = VG_SAFE_STATIC_CAST(const WeaponBehaviour, _object);
    return WeaponType::Pistol != weapon->getWeaponType();
}

//--------------------------------------------------------------------------------------
bool WeaponBehaviour::registerProperties(IClassDesc & _desc)
{
    super::registerProperties(_desc);

    registerPropertyEnum(WeaponBehaviour, WeaponType, m_weaponType, "Type");
    setPropertyDescription(WeaponBehaviour, m_weaponType, "Weapon type");

    registerProperty(WeaponBehaviour, m_projectile, "Projectile");
    setPropertyHiddenCallback(WeaponBehaviour, m_projectile, IsProjectilePropertyHidden);

    return true;
}

//--------------------------------------------------------------------------------------
void WeaponBehaviour::OnPlay()
{
    super::OnPlay();
    const auto & options = Game::get()->Engine().GetOptions();
    auto enemyTag = options->GetGameObjectTag("Enemy");
    auto breakableTag = options->GetGameObjectTag("Breakable");
    m_hitTags = enemyTag | breakableTag;
}

//--------------------------------------------------------------------------------------
void WeaponBehaviour::OnTriggerEnter(vg::core::IGameObject * _other)
{
    if (asBool(m_hitTags & _other->GetTags()))
    {
        if (auto * owner = GetOwner().get<IGameObject>())
        {
            if (auto * attacker = owner->GetComponentT<CharacterBehaviour>())
            {
                if (attacker->getFightState() == FightState::Hit)
                {
                    if (auto * defender = _other->GetComponentInParentsT<EntityBehaviour>())
                        defender->TakeHit(attacker, this);
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


