#pragma once
#include "Behaviour/Entity/Item/ItemBehaviour.h"

vg_enum_class(ChestState, vg::core::u8,
    Default = 0,
    Destroyed
);

class BreakableBehaviour final : public ItemBehaviour
{
public:
    VG_CLASS_DECL(BreakableBehaviour, ItemBehaviour);

    BreakableBehaviour(const vg::core::string & _name, vg::core::IObject * _parent);
    ~BreakableBehaviour();

    void                    OnPlay              () final override;
    bool                    TakeHit             (CharacterBehaviour * _attacker, ItemBehaviour * _weapon) final override;

protected:
    ChestType               m_chestType = ChestType::Default;
    ChestState              m_chestState = ChestState::Default;

    bool                    m_useDestroyAnim = false;
    vg::core::string        m_destroyAnimName;

    bool                    m_useDestroySound = false;
    vg::core::string        m_destroySound;
};
