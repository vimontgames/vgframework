#pragma once
#include "game_consts.h"
#include "core/Object/ObjectHandle.h"
#include "core/Component/Behaviour/Behaviour.h"

class ItemBehaviour : public vg::core::Behaviour
{
public:
    VG_CLASS_DECL(ItemBehaviour, vg::core::Behaviour);

    ItemBehaviour(const vg::core::string & _name, vg::core::IObject * _parent, ItemType _itemType);
    ~ItemBehaviour();

    void                                    OnPlay() override;
    void                                    OnStop() override;

    virtual void                            SetOwner(vg::core::IGameObject * _object);
    virtual const vg::core::ObjectHandle &  GetOwner() const;
    virtual bool                            CanPick() const;

private:
    ItemType                                m_itemType;
    vg::core::ObjectHandle                  m_owner;
};
