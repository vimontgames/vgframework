#pragma once
#include "Behaviour/Entity/Item/ItemBehaviour.h"

class ChestBehaviour final : public ItemBehaviour
{
public:
    VG_CLASS_DECL(ChestBehaviour, ItemBehaviour);

    ChestBehaviour(const vg::core::string & _name, vg::core::IObject * _parent);
    ~ChestBehaviour();

    void                    OnPlay              () final override;
    bool                    TakeHit             (CharacterBehaviour * _attacker, ItemBehaviour * _weapon) final override;

protected:
    ChestType               m_chestType = ChestType::Default;
};
