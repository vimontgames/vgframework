#pragma once
#include "game_consts.h"
#include "Behaviour/Entity/EntityBehaviour.h"

class ItemBehaviour;
class HealthBarBehaviour;

class CharacterBehaviour : public EntityBehaviour
{
public:
    VG_CLASS_DECL(CharacterBehaviour, EntityBehaviour);

    CharacterBehaviour(const vg::core::string & _name, vg::core::IObject * _parent, CharacterType _characterType = CharacterType::Neutral);
    ~CharacterBehaviour();

    void                    OnEnable() override;
    void                    OnDisable() override;

    void                    OnPlay() override;
    void                    OnStop() override;

    void                    FixedUpdate(const Context & _context) override;
    void                    Update(const Context & _context) override;

    void                    addScore(vg::core::i32 _points);
    bool                    TakeHit(CharacterBehaviour * _attacker, ItemBehaviour * _weapon = nullptr) override;

    void                    OnDeath(const Context & _context);

    VG_INLINE bool          isActive() const;
    VG_INLINE MoveState     getMoveState() const;
    VG_INLINE FightState    getFightState() const;
    VG_INLINE float         getHealth() const;

protected:
    void                    initAnimations();
    void                    initSounds();
    void                    initHealthBar();

    void                    playMoveAnim(MoveState _state, bool _loop = false);

    void                    playFightAnim(FightState _state, bool _loop = false);
    void                    stopFightAnim(FightState _state);

    void                    playSound(SoundState _sound);

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

    MoveState               m_moveState = MoveState::Idle;
    vg::core::uint          m_moveAnim[vg::core::enumCount<MoveState>()];

    FightState              m_fightState = FightState::None;
    vg::core::uint          m_fightAnim[vg::core::enumCount<FightState>()];

    SoundState              m_soundState = SoundState::None;
    vg::core::uint          m_sound[vg::core::enumCount<SoundState>()];

    float                   m_speedCurrent = 0.0f;
    vg::core::float2        m_velocitySmoothdamp = vg::core::float2(0, 0);
    float                   m_velocityNorm = 0;

    float                   m_targetRotation = 0.0f;
    float                   m_currentRotation = 0.0f;
    float                   m_rotationSmoothdamp = 0.0f;

    vg::core::float3        m_startPos = (vg::core::float3)0.0f; 

    HealthBarBehaviour *    m_healthBar = nullptr;
};

#if VG_ENABLE_INLINE
#include "CharacterBehaviour.inl"
#endif
