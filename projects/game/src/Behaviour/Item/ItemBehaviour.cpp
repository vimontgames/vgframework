#include "Precomp.h"
#include "ItemBehaviour.h"

#if !VG_ENABLE_INLINE
#include "ItemBehaviour.inl"
#endif

#include "Ball/BallBehaviour.hpp"
#include "Weapon/WeaponBehaviour.hpp"

VG_REGISTER_COMPONENT_CLASS(ItemBehaviour, "Item", "Game", "Basic game item", vg::editor::style::icon::Script, 0);

//--------------------------------------------------------------------------------------
ItemBehaviour::ItemBehaviour(const vg::core::string & _name, vg::core::IObject * _parent, ItemType _itemType) :
    super(_name, _parent),
    m_itemType(_itemType)
{
    // Default item does not implement update
    // Child classes derived from 'Item' should explicitely enable 'Update' if needed
    EnableUpdateFlags(UpdateFlags::Update, false);
}

//--------------------------------------------------------------------------------------
ItemBehaviour::~ItemBehaviour()
{

}

//--------------------------------------------------------------------------------------
bool ItemBehaviour::registerProperties(IClassDesc & _desc)
{
    super::registerProperties(_desc);

    registerProperty(ItemBehaviour, m_damage, "Damage");
    setPropertyRange(ItemBehaviour, m_damage, float2(0, 100));
    setPropertyDescription(ItemBehaviour, m_damage, "Amount of damage inflicted");

    registerProperty(ItemBehaviour, m_push, "Push");
    setPropertyRange(ItemBehaviour, m_push, float2(0, 100));
    setPropertyDescription(ItemBehaviour, m_damage, "Amount of force applied in the direction of the hit");

    registerPropertyEx(ItemBehaviour, m_owner, "Owner", vg::core::PropertyFlags::NotSaved);

    return true;
}

//--------------------------------------------------------------------------------------
void ItemBehaviour::OnPlay()
{
    super::OnPlay();
    Game::get()->addItem(m_itemType, this);
}

//--------------------------------------------------------------------------------------
void ItemBehaviour::OnStop()
{
    Game::get()->removeItem(m_itemType, this);
    m_owner.clear();
    super::OnStop();
}

//--------------------------------------------------------------------------------------
void ItemBehaviour::SetOwner(vg::core::IGameObject * _object)
{
    m_owner.set(_object);
}

//--------------------------------------------------------------------------------------
const vg::core::ObjectHandle & ItemBehaviour::GetOwner() const
{
    return m_owner;
}

//--------------------------------------------------------------------------------------
CharacterBehaviour * ItemBehaviour::GetOwnerCharacter() const
{
    if (auto * owner = m_owner.get<IGameObject>())
        return owner->GetComponentT<CharacterBehaviour>();
  
    return nullptr;
}

//--------------------------------------------------------------------------------------
bool ItemBehaviour::CanPick() const
{
    return nullptr != m_owner.getObject();
}

//--------------------------------------------------------------------------------------
// When player collides with the ball, then is become the new most recent player
//--------------------------------------------------------------------------------------
void ItemBehaviour::OnCollisionEnter(vg::core::IGameObject * _other)
{
    if (auto * player = _other->GetComponentT<PlayerBehaviour>())
    {
        if (player->isActive())
             SetOwner(player->GetGameObject());
    }
    else if (auto * enemy = _other->GetComponentT<EnemyBehaviour>())
    {
        enemy->takeHit(nullptr, this);
    }
}