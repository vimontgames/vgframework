#pragma once
#include "game_consts.h"
#include "core/Component/Behaviour/Behaviour.h"

class CharacterBehaviour : public vg::core::Behaviour
{
public:
    VG_CLASS_DECL(CharacterBehaviour, vg::core::Behaviour);

    CharacterBehaviour(const vg::core::string & _name, vg::core::IObject * _parent, CharacterType _characterType);
    ~CharacterBehaviour();

    void                    OnEnable() override;
    void                    OnDisable() override;

    void                    OnPlay() override;
    void                    OnStop() override;

    void                    FixedUpdate(const Context & _context) override;
    void                    Update(const Context & _context) override;

    bool                    isActive() const { return m_isActive; }

    void                    addScore(vg::core::i32 _points);

protected:
    void                    PlayAnim(CharacterPrimaryState _state, bool _loop = false);
    void                    PlayAnim(CharacterSecondaryState _state, bool _loop = false);
    void                    StopAnim(CharacterSecondaryState _state);

protected:
    bool                    m_isActive = false;
    CharacterType           m_characterType;

    vg::core::uint          m_life = 1;
    float                   m_hp = 100.0f;
    vg::core::i32           m_score = 0;

    float                   m_walkSpeed = 1.5f;
    float                   m_runSpeed = 3.0f;
    float                   m_jumpSpeed = 3.0f;
    float                   m_runJumpSpeed = 4.0f;

    CharacterPrimaryState   m_primaryState = CharacterPrimaryState::Idle;
    vg::core::uint          m_primaryAnim[vg::core::enumCount<CharacterPrimaryState>()];

    CharacterSecondaryState m_secondaryState = CharacterSecondaryState::None;
    vg::core::uint          m_secondaryAnim[vg::core::enumCount<CharacterSecondaryState>()];

    float                   m_currentSpeed = 0.0f;
    float                   m_currentRotation = 0.0f;
    vg::core::float2        m_velocitySmoothdamp = vg::core::float2(0, 0);

    vg::core::float3        m_startPos = (vg::core::float3)0.0f; 
};
