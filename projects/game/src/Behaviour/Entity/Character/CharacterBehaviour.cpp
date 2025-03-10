#include "Precomp.h"
#include "CharacterBehaviour.h"
#include "core/IWorld.h"
#include "core/Math/Math.h"
#include "core/string/string.h"
#include "core/GameObject/GameObject.h"
#include "renderer/IRenderer.h"
#include "renderer/IDebugDraw.h"
#include "engine/IEngineOptions.h"
#include "Behaviour/Entity/Item/Ball/BallBehaviour.h"
#include "Behaviour/Entity/Item/Weapon/WeaponBehaviour.h"
#include "Behaviour/HealthBar/HealthBarBehaviour.h"
#include "Game.h"

#if !VG_ENABLE_INLINE
#include "CharacterBehaviour.inl"
#endif

#include "Player/PlayerBehaviour.hpp"
#include "Enemy/EnemyBehaviour.hpp"

using namespace vg::core;
using namespace vg::engine;

// Component base class is not being exposed but still needs to be registered for custom RTTI
VG_REGISTER_OBJECT_CLASS(CharacterBehaviour, "Character");

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

    registerProperty(CharacterBehaviour, m_life, "Life");
    registerProperty(CharacterBehaviour, m_hp, "HP");

    registerProperty(CharacterBehaviour, m_walkSpeed, "Walk");
    registerProperty(CharacterBehaviour, m_runSpeed, "Run");
    registerProperty(CharacterBehaviour, m_jumpSpeed, "Jump (small)");
    registerProperty(CharacterBehaviour, m_runJumpSpeed, "Jump (big)");

    registerPropertyGroupBegin(CharacterBehaviour, "Character debug");
    {
        registerPropertyEx(CharacterBehaviour, m_isActive, "Active", vg::core::PropertyFlags::Transient);
        registerPropertyEx(CharacterBehaviour, m_score, "Score", vg::core::PropertyFlags::Transient);
        registerPropertyEnumEx(CharacterBehaviour, MoveState, m_moveState, "Move", vg::core::PropertyFlags::Transient);
        registerPropertyEnumEx(CharacterBehaviour, FightState, m_fightState, "Fight", vg::core::PropertyFlags::Transient);
        registerPropertyEnumEx(CharacterBehaviour, SoundState, m_soundState, "Sound", vg::core::PropertyFlags::Transient);
        registerPropertyEx(CharacterBehaviour, m_speedCurrent, "Speed", vg::core::PropertyFlags::Transient);
        registerPropertyEx(CharacterBehaviour, m_velocityNorm, "Velocity", vg::core::PropertyFlags::Transient);
        registerPropertyEx(CharacterBehaviour, m_currentRotation, "Rotation", vg::core::PropertyFlags::Transient);
    }
    registerPropertyGroupEnd(CharacterBehaviour);

    return true;
}

//--------------------------------------------------------------------------------------
void CharacterBehaviour::OnEnable()
{
    super::OnEnable();

    initAnimations();
    initSounds();
    initHealthBar();
}

//--------------------------------------------------------------------------------------
void CharacterBehaviour::initAnimations()
{
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
}

//--------------------------------------------------------------------------------------
void CharacterBehaviour::initSounds()
{
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
void CharacterBehaviour::initHealthBar()
{
    if (auto * healthBar = GetGameObject()->GetComponentInChildrenT<HealthBarBehaviour>())
        healthBar->init(m_hp);
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

    m_isActive = false;
    m_score = 0;
    m_moveState = MoveState::Idle;
    m_fightState = FightState::None;
    m_soundState = SoundState::None;
    m_speedCurrent = 0;
    m_velocityNorm = 0;
    m_currentRotation = 0;
}

//--------------------------------------------------------------------------------------
void CharacterBehaviour::playMoveAnim(MoveState _state, bool _loop)
{
    if (IAnimationComponent * animationComponent = GetGameObject()->GetComponentT<IAnimationComponent>())
        animationComponent->PlayAnimation(m_moveAnim[asInteger(_state)], _loop);
}

//--------------------------------------------------------------------------------------
void CharacterBehaviour::playFightAnim(FightState _state, bool _loop)
{
    if (IAnimationComponent * animationComponent = GetGameObject()->GetComponentT<IAnimationComponent>())
        animationComponent->PlayAnimation(m_fightAnim[asInteger(_state)], _loop);
}

//--------------------------------------------------------------------------------------
void CharacterBehaviour::stopFightAnim(FightState _state)
{
    if (IAnimationComponent * animationComponent = GetGameObject()->GetComponentT<IAnimationComponent>())
        animationComponent->StopAnimation(m_fightAnim[asInteger(_state)]);
}

//--------------------------------------------------------------------------------------
void CharacterBehaviour::playSound(SoundState _sound)
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
        OnDeath(_context);
}

//--------------------------------------------------------------------------------------
void CharacterBehaviour::OnDeath(const Context & _context)
{
    if (m_life > 0)
    {
        m_life--;

        if (m_life > 0)
        {
            if (auto * charaController = _context.m_gameObject->GetComponentT<vg::engine::ICharacterControllerComponent>())
                charaController->SetPosition(m_startPos + float3(0, 0, 16));

            m_hp = 100;
        }
    }
}

//--------------------------------------------------------------------------------------
void CharacterBehaviour::addScore(vg::core::i32 _points)
{
    m_score += _points;
}

//--------------------------------------------------------------------------------------
bool CharacterBehaviour::TakeHit(CharacterBehaviour * _attacker, ItemBehaviour * _weapon)
{
    if (m_moveState != MoveState::Hurt)
    {
        if (m_moveState == MoveState::Die)
            return false;

        auto * go = GetGameObject();
        float3 hitDir;

        if (nullptr != _attacker)
        {
            auto attackerGO = _attacker->GetGameObject();
            hitDir = normalize(go->GetGlobalMatrix()[3].xyz - attackerGO->GetGlobalMatrix()[3].xyz);

            if (_weapon)
                VG_INFO("[Character] \"%s\" was hit by \"%s\" with weapon \"%s\"", go->GetName().c_str(), attackerGO->GetName().c_str(), _weapon->GetGameObject()->GetName().c_str());
            else
                VG_INFO("[Character] \"%s\" was hit by \"%s\"", go->GetName().c_str(), attackerGO->GetName().c_str());
        }
        else
        {
            auto weaponGO = _weapon->GetGameObject();
            hitDir = normalize(go->GetGlobalMatrix()[3].xyz - weaponGO->GetGlobalMatrix()[3].xyz);

            VG_INFO("[Character] \"%s\" was hit by \"%s\"", go->GetName().c_str(), weaponGO->GetName().c_str());
        }

        float damage = _weapon ? _weapon->getDamage() : 1.0f;
        m_hp = max(0.0f, m_hp - damage);

        if (damage > 0.0f)
        {
            if (auto * healthBar = GetGameObject()->GetComponentInChildrenT<HealthBarBehaviour>())
                healthBar->setHP(m_hp);

            float push = _weapon ? _weapon->getPush() : 0.0f;

            if (m_hp > 0)
            {
                m_moveState = MoveState::Hurt;
                playSound(SoundState::Hurt);
            }
            else
            {
                m_moveState = MoveState::Die;
                playSound(SoundState::Die);
            }

            if (push > 0)
            {
                if (auto * characterController = go->GetComponentT<ICharacterControllerComponent>())
                    characterController->SetVelocity(characterController->GetVelocity() + hitDir * push);
            }
        }

        return true; // hit was taken
    }

    return false;
}