#include "BreakableBehaviour.h"
#include "Game.h"
#include "Behaviour/Entity/Item/ItemBehaviour.h"
#include "Behaviour/Entity/Character/CharacterBehaviour.h"

using namespace vg::core;
using namespace vg::engine;

VG_REGISTER_COMPONENT_CLASS(BreakableBehaviour, "Breakable", "Game", "A breakable object that can be destroyed", vg::editor::style::icon::Script, 0);

//--------------------------------------------------------------------------------------
BreakableBehaviour::BreakableBehaviour(const string & _name, IObject * _parent) :
    super(_name, _parent, ItemType::Chest)
{
    EnableUpdateFlags(vg::core::UpdateFlags::Update, false);
}

//--------------------------------------------------------------------------------------
BreakableBehaviour::~BreakableBehaviour()
{

}

//--------------------------------------------------------------------------------------
bool BreakableBehaviour::registerProperties(IClassDesc & _desc)
{
    super::registerProperties(_desc);

    registerPropertyEnum(BreakableBehaviour, ChestType, m_chestType, "Type");
    setPropertyDescription(BreakableBehaviour, m_chestType, "Chest type");

    registerPropertyGroupBegin(BreakableBehaviour, "OnDestroy")
    {
        registerOptionalProperty(BreakableBehaviour, m_useDestroyAnim, m_destroyAnimName, "Animation");
        setPropertyDescription(BreakableBehaviour, m_destroyAnimName, "Name of the animation to play on destroy (an Animation component is required)");

        registerOptionalProperty(BreakableBehaviour, m_useDestroySound, m_destroySound, "Sound");
        setPropertyDescription(BreakableBehaviour, m_destroyAnimName, "Name of the sound to play on destroy (a Sound component is required)");
    }
    registerPropertyGroupEnd(BreakableBehaviour);

    registerPropertyEnumEx(BreakableBehaviour, ChestState, m_chestState, "State", vg::core::PropertyFlags::Transient);
    setPropertyDescription(BreakableBehaviour, m_chestState, "Chest state");

    return true;
}

//--------------------------------------------------------------------------------------
void BreakableBehaviour::OnPlay()
{
    super::OnPlay();

    // Reset chest state (as it's not serialized so it's not restored on restart)
    m_chestState = ChestState::Default;
}

//--------------------------------------------------------------------------------------
bool BreakableBehaviour::TakeHit(CharacterBehaviour * _attacker, ItemBehaviour * _weapon)
{
    if (m_chestState == ChestState::Default)
    {
        auto * thisGameObject = GetGameObject();
        float3 hitDir;

        if (nullptr != _attacker)
        {
            auto * attackerGO = _attacker->GetGameObject();
            hitDir = normalize(thisGameObject->GetGlobalMatrix()[3].xyz - attackerGO->GetGlobalMatrix()[3].xyz);

            if (_weapon)
                VG_INFO("[Entity] \"%s\" was hit by \"%s\" with \"%s\"", thisGameObject->GetName().c_str(), attackerGO->GetName().c_str(), _weapon->GetGameObject()->GetName().c_str());
            else
                VG_INFO("[Entity] \"%s\" was hit by \"%s\"", thisGameObject->GetName().c_str(), attackerGO->GetName().c_str());

            m_chestState = ChestState::Destroyed;

            if (m_useDestroyAnim)
            {
                if (auto * animComp = thisGameObject->GetComponentT<IAnimationComponent>())
                {
                    uint animIndex = animComp->GetAnimationIndex(m_destroyAnimName);
                    if (-1 != animIndex)
                        animComp->PlayAnimation(animIndex, false);
                    else
                        VG_WARNING("[Chest] Missing OnDestroy animation \"%s\"", m_destroyAnimName.c_str());
                }
                else
                {
                    VG_WARNING("[Chest] Missing IAnimationComponent");
                }
            }

            if (m_useDestroySound)
            {
                if (auto * soundComponent = GetGameObject()->GetComponentT<ISoundComponent>())
                {
                    uint soundIndex = soundComponent->GetSoundIndex(m_destroySound);
                    if (-1 != soundIndex)
                        soundComponent->Play(soundIndex);
                    else
                        VG_WARNING("[Chest] Missing OnDestroy sound \"%s\"", m_destroyAnimName.c_str());
                }
                else
                {
                    VG_WARNING("[Chest] Missing ISoundComponent");
                }
            }

            // Disable physics
            if (auto * physicsBodyComp = thisGameObject->GetComponentT<vg::engine::IPhysicsBodyComponent>())
                physicsBodyComp->Enable(false);

            return true; // hit was taken
        }
        else
        {
            auto * weaponGO = _weapon->GetGameObject();
            hitDir = normalize(thisGameObject->GetGlobalMatrix()[3].xyz - weaponGO->GetGlobalMatrix()[3].xyz);

            VG_INFO("[Entity] \"%s\" was hit by \"%s\"", thisGameObject->GetName().c_str(), weaponGO->GetName().c_str());
        }        
    }

    return false;
}



