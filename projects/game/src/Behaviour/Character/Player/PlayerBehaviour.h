#pragma once
#include "Behaviour/Character/CharacterBehaviour.h"

class PlayerBehaviour final : public CharacterBehaviour
{
public:
    VG_CLASS_DECL(PlayerBehaviour, CharacterBehaviour);

    PlayerBehaviour     (const vg::core::string & _name, vg::core::IObject * _parent);
    ~PlayerBehaviour    ();

    void                OnPlay              () final override;
    void                OnStop              () final override;

    void                FixedUpdate         (float _dt) override;
    void                Update              (float _dt) override;

private:
    vg::core::InputType m_controllerType    = vg::core::InputType::Joypad;
    vg::core::u8        m_controllerIndex   = 0;
};