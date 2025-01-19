#pragma once
#include "Behaviour/Entity/Item/ItemBehaviour.h"

vg_enum_class(BreakableState, vg::core::u8,
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
    void                    Update              (const Context & _context) final override;
    bool                    TakeHit             (CharacterBehaviour * _attacker, ItemBehaviour * _weapon) final override;
    void                    OnTriggerEnter      (vg::core::IGameObject * _other);

protected:
    BreakableType           m_breakableType = BreakableType::LootBox;
    BreakableState          m_breakableState = BreakableState::Default;

    vg::core::ObjectHandle  m_default;
    bool                    m_useRotation = false;
    vg::core::float3        m_rotation = vg::core::float3(0.0f, 0.0f, 1.0f);

    vg::core::ObjectHandle  m_destroyed;

    //bool                    m_useDestroyAnim = false;
    //vg::core::string        m_destroyAnimName;

    bool                    m_useDestroySound = false;
    vg::core::string        m_destroySound;

    bool                    m_useImpulse = false;
    vg::core::float3        m_impulse = float3(0,0,0);
};
