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
CharacterBehaviour::CharacterBehaviour(const string& _name, IObject* _parent) :
    super(_name, _parent)
{
    SetUpdateFlags(UpdateFlags::FixedUpdate | UpdateFlags::Update);

    for (uint i = 0; i < vg::core::countof(m_anim); ++i)
        m_anim[i] = -1;
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

        registerPropertyGroupBegin(CharacterBehaviour, "Debug");
        {
            registerPropertyEx(CharacterBehaviour, m_isActive, "Active", vg::core::PropertyFlags::NotSaved);
            registerPropertyEnumEx(CharacterBehaviour, CharacterState, m_state, "State", vg::core::PropertyFlags::NotSaved);
            registerPropertyEx(CharacterBehaviour, m_currentSpeed, "Speed", vg::core::PropertyFlags::NotSaved);
            registerPropertyEx(CharacterBehaviour, m_currentRotation, "Rot", vg::core::PropertyFlags::NotSaved);
        }
        registerPropertyGroupEnd(CharacterBehaviour);
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
        m_anim[CharacterState::Idle] = animationComponent->GetAnimationIndex("Idle");
        m_anim[CharacterState::Walking] = animationComponent->GetAnimationIndex("Walking");
        m_anim[CharacterState::Running] = animationComponent->GetAnimationIndex("Running");
        m_anim[CharacterState::Jumping] = animationComponent->GetAnimationIndex("Jump");
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

    m_startPos = GetGameObject()->getGlobalMatrix()[3].xyz;
}

//--------------------------------------------------------------------------------------
void CharacterBehaviour::OnStop()
{
    super::OnStop();
}

//--------------------------------------------------------------------------------------
void CharacterBehaviour::PlayAnim(CharacterState _state, bool _loop)
{
    if (IAnimationComponent * animationComponent = GetGameObject()->GetComponentT<IAnimationComponent>())
        animationComponent->PlayAnimation(m_anim[_state], _loop, 1.0f);
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