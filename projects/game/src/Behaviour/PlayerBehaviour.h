#pragma once
#include "core/Component/Behaviour/Behaviour.h"

enum PlayerState : vg::core::u8
{
    Idle = 0,
    Walking,
    Running,
    Jumping
};

class PlayerBehaviour final: public vg::core::Behaviour
{
public:
    VG_CLASS_DECL(PlayerBehaviour, vg::core::Behaviour);

                    PlayerBehaviour     (const vg::core::string & _name, vg::core::IObject * _parent);
                    ~PlayerBehaviour    ();

    void            OnPlay              () final override;
    void            OnStop              () final override;

    void            FixedUpdate         (float _dt) override;
    void            Update              (float _dt) override;

private:
    void            PlayAnim            (PlayerState _state, bool _loop = false);

private:
    vg::core::u8    m_joypadID          = 0;
    float           m_walkSpeed         = 1.5f;
    float           m_runSpeed          = 3.0f;
    float           m_jumpSpeed         = 3.0f;
    float           m_runJumpSpeed      = 4.0f;

    PlayerState     m_state             = PlayerState::Idle;
    float           m_currentSpeed      = 0.0f;
    float           m_currentRotation   = 0.0f;

    vg::core::uint  m_anim[vg::core::enumCount<PlayerState>()];
};
