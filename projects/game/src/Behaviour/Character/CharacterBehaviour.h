#pragma once
#include "core/Component/Behaviour/Behaviour.h"

enum CharacterState : vg::core::u8
{
    Idle = 0,
    Walking,
    Running,
    Jumping
};

class CharacterBehaviour : public vg::core::Behaviour
{
public:
    VG_CLASS_DECL(CharacterBehaviour, vg::core::Behaviour);

    CharacterBehaviour(const vg::core::string& _name, vg::core::IObject* _parent);
    ~CharacterBehaviour();

    void                OnPlay() override;
    void                OnStop() override;

    void                FixedUpdate(float _dt) override;
    void                Update(float _dt) override;

protected:
    void                PlayAnim(CharacterState _state, bool _loop = false);

protected:
    float               m_walkSpeed = 1.5f;
    float               m_runSpeed = 3.0f;
    float               m_jumpSpeed = 3.0f;
    float               m_runJumpSpeed = 4.0f;

    CharacterState      m_state = CharacterState::Idle;
    float               m_currentSpeed = 0.0f;
    float               m_currentRotation = 0.0f;

    vg::core::float3    m_startPos = (vg::core::float3)0.0f;
    vg::core::uint      m_anim[vg::core::enumCount<CharacterState>()];
};
