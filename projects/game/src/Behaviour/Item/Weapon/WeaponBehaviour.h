#pragma once
#include "Behaviour/Item/ItemBehaviour.h"

class CharacterBehaviour;

class WeaponBehaviour final : public ItemBehaviour
{
public:
    VG_CLASS_DECL(WeaponBehaviour, ItemBehaviour);

    WeaponBehaviour(const vg::core::string & _name, vg::core::IObject * _parent);
    ~WeaponBehaviour();

    void                OnPlay              () final override;
    void                OnTriggerEnter      (vg::core::IGameObject * _other) final override;
    void                OnCollisionEnter    (vg::core::IGameObject * _other) final override;
    void                SetOwner            (vg::core::IGameObject * _object) final override;

private:
    vg::core::Tag       m_ennemyTag         = (vg::core::Tag)0x0;
};
