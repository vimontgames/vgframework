#include "Precomp.h"
#include "CharacterBehaviour.h"
#include "Game.h"
#include "engine/ICharacterControllerComponent.h"
#include "editor/Editor_Consts.h"
#include "core/GameObject/GameObject.h"

#if !VG_ENABLE_INLINE
#include "CharacterBehaviour.inl"
#endif

#include "Player/PlayerBehaviour.hpp"
#include "Enemy/EnemyBehaviour.hpp"

using namespace vg::core;
using namespace vg::engine;

VG_REGISTER_ABSTRACT_CLASS(CharacterBehaviour, "CharacterBehaviour");

//--------------------------------------------------------------------------------------
CharacterBehaviour::CharacterBehaviour(const string & _name, IObject * _parent, CharacterType _characterType) :
    super(_name, _parent),
    m_characterType(_characterType)
{
    EnableUpdateFlags(UpdateFlags::FixedUpdate | UpdateFlags::Update);

    for (uint i = 0; i < vg::core::countof(m_moveAnim); ++i)
        m_moveAnim[i] = -1;

    for (uint i = 0; i < vg::core::countof(m_fightAnim); ++i)
        m_fightAnim[i] = -1;
}

//--------------------------------------------------------------------------------------
CharacterBehaviour::~CharacterBehaviour()
{

}

//--------------------------------------------------------------------------------------
bool CharacterBehaviour::registerProperties(IClassDesc& _desc)
{
    super::registerProperties(_desc);

    registerPropertyGroupBegin(CharacterBehaviour, "Character");
    {
        registerProperty(CharacterBehaviour, m_life, "Life");
        registerProperty(CharacterBehaviour, m_hp, "HP");
        registerProperty(CharacterBehaviour, m_score, "Score");

        registerProperty(CharacterBehaviour, m_walkSpeed, "Walk");
        registerProperty(CharacterBehaviour, m_runSpeed, "Run");
        registerProperty(CharacterBehaviour, m_jumpSpeed, "Small Jump");
        registerProperty(CharacterBehaviour, m_runJumpSpeed, "Big Jump");

        //registerPropertyGroupBegin(CharacterBehaviour, "Debug");
        {
            registerPropertyEx(CharacterBehaviour, m_isActive, "Active", vg::core::PropertyFlags::NotSaved);
            registerPropertyEx(CharacterBehaviour, m_speedCurrent, "Speed", vg::core::PropertyFlags::NotSaved);
            registerPropertyEx(CharacterBehaviour, m_velocityNorm, "Velocity", vg::core::PropertyFlags::NotSaved);
            registerPropertyEx(CharacterBehaviour, m_currentRotation, "Rotation", vg::core::PropertyFlags::NotSaved);

            registerPropertyEnumEx(CharacterBehaviour, MoveState, m_moveState, "Move", vg::core::PropertyFlags::NotSaved);
            registerPropertyEnumEx(CharacterBehaviour, FightState, m_fightState, "Fight", vg::core::PropertyFlags::NotSaved);
            registerPropertyEnumEx(CharacterBehaviour, SoundState, m_soundState, "Sound", vg::core::PropertyFlags::NotSaved);
        }
        //registerPropertyGroupEnd(CharacterBehaviour);
    }
    registerPropertyGroupEnd(CharacterBehaviour);

    return true;
}

//--------------------------------------------------------------------------------------
void CharacterBehaviour::OnEnable()
{
    super::OnEnable();

    if (IAnimationComponent * animation = GetGameObject()->GetComponentT<IAnimationComponent>())
    {
        for (uint i = 0; i < enumCount<MoveState>(); ++i)
            m_moveAnim[i] = animation->GetAnimationIndex(asString((MoveState)i));

        for (uint i = 0; i < enumCount<FightState>(); ++i)
        {
            switch ((FightState)i)
            {
                case FightState::None:
                    break;

                default:
                    m_fightAnim[i] = animation->GetAnimationIndex(asString((FightState)i));
                    break;
            }
        }
    }  
    else
    {
        VG_WARNING("[Animation] Character \"%s\" has no Animation component", GetGameObject()->GetName().c_str());
    }

    if (ISoundComponent * sound = GetGameObject()->GetComponentT<ISoundComponent>())
    {
        for (uint i = 0; i < enumCount<SoundState>(); ++i)
        {
            switch ((SoundState)i)
            {
                case SoundState::None:
                    break;

                default:
                    m_sound[i] = sound->GetSoundIndex(asString((SoundState)i));
                    break;
            }
        }
    }
    else
    {
        VG_WARNING("[Sound] Character \"%s\" has no Sound component", GetGameObject()->GetName().c_str());
    }
}

//--------------------------------------------------------------------------------------
void CharacterBehaviour::OnDisable()
{
    super::OnDisable();
}

//--------------------------------------------------------------------------------------
void CharacterBehaviour::OnPlay()
{
    super::OnPlay();
    Game::get()->addCharacter(m_characterType, this);
    m_startPos = GetGameObject()->getGlobalMatrix()[3].xyz;
}

//--------------------------------------------------------------------------------------
void CharacterBehaviour::OnStop()
{
    Game::get()->removeCharacter(m_characterType, this);
    super::OnStop();
}

//--------------------------------------------------------------------------------------
void CharacterBehaviour::PlayMoveAnim(MoveState _state, bool _loop)
{
    if (IAnimationComponent * animationComponent = GetGameObject()->GetComponentT<IAnimationComponent>())
        animationComponent->PlayAnimation(m_moveAnim[asInteger(_state)], _loop);
}

//--------------------------------------------------------------------------------------
void CharacterBehaviour::PlayFightAnim(FightState _state, bool _loop)
{
    if (IAnimationComponent * animationComponent = GetGameObject()->GetComponentT<IAnimationComponent>())
        animationComponent->PlayAnimation(m_fightAnim[asInteger(_state)], _loop);
}

//--------------------------------------------------------------------------------------
void CharacterBehaviour::StopFightAnim(FightState _state)
{
    if (IAnimationComponent * animationComponent = GetGameObject()->GetComponentT<IAnimationComponent>())
        animationComponent->StopAnimation(m_fightAnim[asInteger(_state)]);
}

//--------------------------------------------------------------------------------------
void CharacterBehaviour::PlaySound(SoundState _sound)
{
    if (auto * soundComponent = GetGameObject()->GetComponentT<ISoundComponent>())
    {
        auto sound = m_sound[asInteger(_sound)];
        soundComponent->Play(sound);
        m_soundState = _sound;
    }
}

//--------------------------------------------------------------------------------------
void CharacterBehaviour::FixedUpdate(const Context & _context)
{
    super::FixedUpdate(_context);
}

//--------------------------------------------------------------------------------------
void CharacterBehaviour::Update(const Context & _context)
{
    auto world = _context.m_gameObject->getGlobalMatrix();
    if (world[3].z < -32.0f)
    {
        if (m_life > 0)
        {
            m_life--;

            if (m_life > 0)
            {
                if (auto * charaController = _context.m_gameObject->GetComponentT<vg::engine::ICharacterControllerComponent>())
                    charaController->SetPosition(m_startPos + float3(0, 0, 16));
            }
        }
    }
}

//--------------------------------------------------------------------------------------
void CharacterBehaviour::addScore(vg::core::i32 _points)
{
    m_score += _points;
}

//--------------------------------------------------------------------------------------
bool CharacterBehaviour::takeHit(CharacterBehaviour * _attacker, ItemBehaviour * _weapon)
{
    if (m_moveState != MoveState::Hurt)
    {
        if (_weapon)
            VG_WARNING("[Enemy] \"%s\" was hit by \"%s\" with \"%s\"", this->GetGameObject()->GetName().c_str(), _attacker->GetGameObject()->GetName().c_str(), _weapon->GetGameObject()->GetName().c_str());
        else
            VG_WARNING("[Enemy] \"%s\" was hit by \"%s\"", this->GetGameObject()->GetName().c_str(), _attacker->GetGameObject()->GetName().c_str());

        PlaySound(SoundState::TakeDamage);

        m_moveState = MoveState::Hurt;
        return true; // hit was taken
    }

    return false;
}