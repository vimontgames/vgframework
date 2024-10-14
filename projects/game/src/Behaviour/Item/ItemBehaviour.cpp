#include "Precomp.h"
#include "ItemBehaviour.h"

#if !VG_ENABLE_INLINE
#include "ItemBehaviour.inl"
#endif

#include "Ball/BallBehaviour.hpp"
#include "Weapon/WeaponBehaviour.hpp"

VG_REGISTER_ABSTRACT_CLASS(ItemBehaviour, "ItemBehaviour");

//--------------------------------------------------------------------------------------
ItemBehaviour::ItemBehaviour(const vg::core::string & _name, vg::core::IObject * _parent, ItemType _itemType) :
    super(_name, _parent),
    m_itemType(_itemType)
{

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
bool ItemBehaviour::CanPick() const
{
    return nullptr != m_owner.getObject();
}