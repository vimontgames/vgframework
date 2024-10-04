#include "Precomp.h"
#include "CharacterBehaviour.h"
#include "Game.h"
#include "engine/ICharacterControllerComponent.h"
#include "editor/Editor_Consts.h"
#include "core/GameObject/GameObject.h"

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
    SetUpdateFlags(UpdateFlags::FixedUpdate | UpdateFlags::Update);

    for (uint i = 0; i < vg::core::countof(m_primaryAnim); ++i)
        m_primaryAnim[i] = -1;

    for (uint i = 0; i < vg::core::countof(m_secondaryAnim); ++i)
        m_secondaryAnim[i] = -1;
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
            registerPropertyEx(CharacterBehaviour, m_currentSpeed, "Speed", vg::core::PropertyFlags::NotSaved);
            registerPropertyEx(CharacterBehaviour, m_currentRotation, "Rot", vg::core::PropertyFlags::NotSaved);

            registerPropertyEnumEx(CharacterBehaviour, CharacterPrimaryState, m_primaryState, "Primary State", vg::core::PropertyFlags::NotSaved);
            registerPropertyEnumEx(CharacterBehaviour, CharacterSecondaryState, m_secondaryState, "Secondary State", vg::core::PropertyFlags::NotSaved);
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

    IAnimationComponent* animationComponent = GetGameObject()->GetComponentT<IAnimationComponent>();

    if (nullptr != animationComponent)
    {
        for (uint i = 0; i < enumCount<CharacterPrimaryState>(); ++i)
        {
            const auto characterPrimaryState = (CharacterPrimaryState)i;

            switch (characterPrimaryState)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(characterPrimaryState);
                    break;

                case CharacterPrimaryState::Idle:
                    m_primaryAnim[i] = animationComponent->GetAnimationIndex("Idle");
                    break;

                case CharacterPrimaryState::Walking:
                    m_primaryAnim[i] = animationComponent->GetAnimationIndex("Walking");
                    break;

                case CharacterPrimaryState::Running:
                    m_primaryAnim[i] = animationComponent->GetAnimationIndex("Running");
                    break;

                case CharacterPrimaryState::Jumping:
                    m_primaryAnim[i] = animationComponent->GetAnimationIndex("Jump");
                    break;
            }               
        }

        for (uint i = 0; i < enumCount<CharacterSecondaryState>(); ++i)
        {
            const auto characterSecondaryState = (CharacterSecondaryState)i;

            switch (characterSecondaryState)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(characterSecondaryState);
                    break;

                case CharacterSecondaryState::None:
                    break;

                case CharacterSecondaryState::SwordHit:
                    m_secondaryAnim[i] = animationComponent->GetAnimationIndex("SwordHit");
                    break;

                case CharacterSecondaryState::KickBall:
                    m_secondaryAnim[i] = animationComponent->GetAnimationIndex("KickBall");
                    break;
            }
        }
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
void CharacterBehaviour::PlayAnim(CharacterPrimaryState _state, bool _loop)
{
    if (IAnimationComponent * animationComponent = GetGameObject()->GetComponentT<IAnimationComponent>())
        animationComponent->PlayAnimation(m_primaryAnim[_state], _loop);
}

//--------------------------------------------------------------------------------------
void CharacterBehaviour::PlayAnim(CharacterSecondaryState _state, bool _loop)
{
    if (IAnimationComponent * animationComponent = GetGameObject()->GetComponentT<IAnimationComponent>())
        animationComponent->PlayAnimation(m_secondaryAnim[_state], _loop);
}

//--------------------------------------------------------------------------------------
void CharacterBehaviour::StopAnim(CharacterSecondaryState _state)
{
    if (IAnimationComponent * animationComponent = GetGameObject()->GetComponentT<IAnimationComponent>())
        animationComponent->StopAnimation(m_secondaryAnim[_state]);
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