#include "Precomp.h"
#include "CharacterBehaviour.h"
#include "Game.h"
#include "engine/ICharacterControllerComponent.h"
#include "editor/Editor_Consts.h"
#include "core/GameObject/GameObject.h"

using namespace vg::core;
using namespace vg::engine;

VG_REGISTER_COMPONENT_CLASS(CharacterBehaviour, "Character", "Game", "Base Character Behaviour", vg::editor::style::icon::Script);

//--------------------------------------------------------------------------------------
CharacterBehaviour::CharacterBehaviour(const string& _name, IObject* _parent) :
    super(_name, _parent)
{
    SetUpdateFlags(UpdateFlags::FixedUpdate | UpdateFlags::Update);
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
        registerProperty(CharacterBehaviour, m_walkSpeed, "Walk");
        registerProperty(CharacterBehaviour, m_runSpeed, "Run");
        registerProperty(CharacterBehaviour, m_jumpSpeed, "Small Jump");
        registerProperty(CharacterBehaviour, m_runJumpSpeed, "Big Jump");

        registerPropertyGroupBegin(CharacterBehaviour, "Debug");
        {
            registerPropertyEnumEx(CharacterBehaviour, CharacterState, m_state, "State", vg::core::IProperty::Flags::NotSaved);
            registerPropertyEx(CharacterBehaviour, m_currentSpeed, "Speed", vg::core::IProperty::Flags::NotSaved);
            registerPropertyEx(CharacterBehaviour, m_currentRotation, "Rot", vg::core::IProperty::Flags::NotSaved);
        }
        registerPropertyGroupEnd(CharacterBehaviour);
    }
    registerPropertyGroupEnd(CharacterBehaviour);

    return true;
}

//--------------------------------------------------------------------------------------
void CharacterBehaviour::OnPlay()
{
    super::OnPlay();

    for (uint i = 0; i < vg::core::countof(m_anim); ++i)
        m_anim[i] = -1;

    IAnimationComponent* animationComponent = GetGameObject()->GetComponentByType<IAnimationComponent>();

    if (nullptr != animationComponent)
    {
        m_anim[CharacterState::Idle] = animationComponent->GetAnimationIndex("Idle");
        m_anim[CharacterState::Walking] = animationComponent->GetAnimationIndex("Walking");
        m_anim[CharacterState::Running] = animationComponent->GetAnimationIndex("Running");
        m_anim[CharacterState::Jumping] = animationComponent->GetAnimationIndex("Jump");
    }

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
    IAnimationComponent* animationComponent = GetGameObject()->GetComponentByType<IAnimationComponent>();

    for (uint i = 0; i < vg::core::countof(m_anim); ++i)
    {
        if (_state != i)
        {
            if (IAnimationResource* anim = animationComponent->GetAnimation(m_anim[i]))
            {
                if (anim->IsPlaying())
                    anim->Stop();
            }
        }
    }

    if (IAnimationResource* anim = animationComponent->GetAnimation(m_anim[_state]))
    {
        if (anim && !anim->IsPlaying())
        {
            if (_loop)
                anim->PlayLoop();
            else
                anim->PlayOnce();
        }
    }
}

//--------------------------------------------------------------------------------------
void CharacterBehaviour::FixedUpdate(float _dt)
{
    super::FixedUpdate(_dt);
}

//--------------------------------------------------------------------------------------
void CharacterBehaviour::Update(float _dt)
{
    auto* go = GetGameObject();
    auto world = go->getGlobalMatrix();
    if (world[3].z < -32.0f)
    {
        if (auto* charaController = go->GetComponentByType<vg::engine::ICharacterControllerComponent>())
            charaController->SetPosition(m_startPos + float3(0, 0, 16));
    }
}