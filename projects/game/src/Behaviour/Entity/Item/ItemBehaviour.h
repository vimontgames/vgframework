#pragma once
#include "Game_consts.h"
#include "core/Object/ObjectHandle.h"
#include "Behaviour/Entity/EntityBehaviour.h"

class CharacterBehaviour;

class ItemBehaviour : public EntityBehaviour
{
public:
    VG_CLASS_DECL(ItemBehaviour, EntityBehaviour);

    ItemBehaviour(const vg::core::string & _name, vg::core::IObject * _parent, ItemType _itemType = ItemType::Default);
    ~ItemBehaviour();

    void                                    OnPlay              () override;
    void                                    OnStop              () override;

    void                                    OnCollisionEnter    (vg::core::IGameObject * _other) override;

    virtual void                            SetOwner            (vg::core::IGameObject * _object);
    virtual const vg::core::ObjectHandle &  GetOwner            () const;
    virtual bool                            CanPick             () const;

    virtual CharacterBehaviour *            GetOwnerCharacter   () const;

    VG_INLINE float                         getDamage           () const;
    VG_INLINE float                         getPush             () const;
    VG_INLINE ItemType                      getItemType         () const;

private:
    float                                   m_damage = 10.0f;
    float                                   m_push = 10.0f;
    ItemType                                m_itemType;
    vg::core::ObjectHandle                  m_owner;
};

#if VG_ENABLE_INLINE
#include "ItemBehaviour.inl"
#endif
