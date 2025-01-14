#pragma once
#include "Behaviour/Entity/Item/ItemBehaviour.h"

class CharacterBehaviour;

class WeaponBehaviour final : public ItemBehaviour
{
public:
    VG_CLASS_DECL(WeaponBehaviour, ItemBehaviour);

    WeaponBehaviour(const vg::core::string & _name, vg::core::IObject * _parent);
    ~WeaponBehaviour();

    void                    OnPlay              () final override;
    void                    OnTriggerEnter      (vg::core::IGameObject * _other) final override;
    void                    OnCollisionEnter    (vg::core::IGameObject * _other) final override;
    void                    SetOwner            (vg::core::IGameObject * _object) final override;

    WeaponType              getWeaponType       () const { return m_weaponType; }
    vg::core::ObjectHandle  getProjectile       () const { return m_projectile; }

protected:
    WeaponType              m_weaponType        = WeaponType::Melee;

private:
    vg::core::Tag           m_hitTags           = (vg::core::Tag)0x0;
    vg::core::ObjectHandle  m_projectile;
};
