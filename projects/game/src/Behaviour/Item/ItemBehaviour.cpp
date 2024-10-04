#include "Precomp.h"
#include "ItemBehaviour.h"

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
bool ItemBehaviour::CanPick() const
{
    return nullptr != m_owner.getObject();
}