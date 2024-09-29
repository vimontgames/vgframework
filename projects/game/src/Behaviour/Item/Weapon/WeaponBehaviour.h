#pragma once
#include "Behaviour/Item/ItemBehaviour.h"

class CharacterBehaviour;

class WeaponBehaviour final : public ItemBehaviour
{
public:
    VG_CLASS_DECL(WeaponBehaviour, ItemBehaviour);

    WeaponBehaviour(const vg::core::string & _name, vg::core::IObject * _parent);
    ~WeaponBehaviour();

    void                    OnPlay() final override;
    void                    Update(const Context & _context) override;
    void                    SetOwner(vg::core::IGameObject * _object) final override;
};
