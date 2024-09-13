#include "Precomp.h"
#include "ItemBehaviour.h"

#include "Ball/BallBehaviour.hpp"

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
    super::OnStop();
}