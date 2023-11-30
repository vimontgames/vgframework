#pragma once
#include "core/Component/Behaviour/Behaviour.h"

enum PlayerState : vg::core::u8
{
    Idle = 0,
    Hello,
    Walking
};

enum Facing : vg::core::u8
{
    None = 0,
    Right,
    Left
};

class PlayerBehaviour : public vg::core::Component
{
public:
    VG_CLASS_DECL(PlayerBehaviour, vg::core::Behaviour);

                        PlayerBehaviour     (const vg::core::string & _name, vg::core::IObject * _parent);
                        ~PlayerBehaviour    ();

    void                Update              (double _dt) override;

private:
    vg::core::u8        m_joypadID = 0;
    PlayerState         m_state = PlayerState::Idle;
    Facing              m_facing;
    float               m_speed = 1.0f;
    bool                m_facingLeft = false;
};
