#pragma once
#include "core/Component/Behaviour/Behaviour.h"

enum PlayerState : vg::core::u8
{
    Idle = 0,
    Walking,
    Running
};

class PlayerBehaviour : public vg::core::Component
{
public:
    VG_CLASS_DECL(PlayerBehaviour, vg::core::Behaviour);

                    PlayerBehaviour     (const vg::core::string & _name, vg::core::IObject * _parent);
                    ~PlayerBehaviour    ();

    void            OnPlay              () final override;

    void            Update              (double _dt) override;

private:
    void            PlayAnim            (PlayerState _state);

private:
    vg::core::u8    m_joypadID  = 0;
    PlayerState     m_state     = PlayerState::Idle;
    float           m_walkSpeed = 1.5f;
    float           m_runSpeed  = 3.0f;

    float           m_dbgSpeed  = 0.0f;
    float           m_dbgRot    = 0.0f;

    vg::core::uint  m_anim[vg::core::enumCount<PlayerState>()];
};
