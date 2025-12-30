#include "BreakableBehaviour.h"
#include "Game.h"
#include "Behaviour/Entity/Item/ItemBehaviour.h"
#include "Behaviour/Entity/Character/CharacterBehaviour.h"
#include "core/Misc/Random/Random.h"

using namespace vg::core;
using namespace vg::engine;

VG_REGISTER_COMPONENT_CLASS(BreakableBehaviour, "Breakable", "Game", "A breakable object that can be destroyed", vg::editor::style::icon::Script, 0);

//--------------------------------------------------------------------------------------
BreakableBehaviour::BreakableBehaviour(const string & _name, IObject * _parent) :
    super(_name, _parent, ItemType::Chest)
{
    EnableUpdateFlags(UpdateFlags::Update, true);
}

//--------------------------------------------------------------------------------------
BreakableBehaviour::~BreakableBehaviour()
{

}

//--------------------------------------------------------------------------------------
bool BreakableBehaviour::registerProperties(IClassDesc & _desc)
{
    super::registerProperties(_desc);

    registerPropertyEnum(BreakableBehaviour, BreakableType, m_breakableType, "Type");
    setPropertyDescription(BreakableBehaviour, m_breakableType, "Chest type");

    registerPropertyGroupBegin(BreakableBehaviour, "Bonus")
    {
        registerProperty(BreakableBehaviour, m_giveLifes, "Life");
        setPropertyDescription(BreakableBehaviour, m_giveLifes, "Amount of life received when broken");

        registerProperty(BreakableBehaviour, m_giveHP, "HP");
        setPropertyDescription(BreakableBehaviour, m_giveHP, "Amount of HP received when broken");

        registerProperty(BreakableBehaviour, m_giveScore, "Score");
        setPropertyDescription(BreakableBehaviour, m_giveScore, "Amount of score received when broken");
    }
    registerPropertyGroupEnd(BreakableBehaviour);

    registerPropertyGroupBegin(BreakableBehaviour, "Default")
    {
        registerProperty(BreakableBehaviour, m_default, "GameObject");
        registerOptionalProperty(BreakableBehaviour, m_useRotation, m_rotation, "Rotation");
    }
    registerPropertyGroupEnd(BreakableBehaviour);

    registerPropertyGroupBegin(BreakableBehaviour, "Destroyed")
    {
        registerProperty(BreakableBehaviour, m_destroyed, "Destroyed");

        //registerOptionalProperty(BreakableBehaviour, m_useDestroyAnim, m_destroyAnimName, "Animation");
        //setPropertyDescription(BreakableBehaviour, m_destroyAnimName, "Name of the animation to play on destroy (an Animation component is required)");

        registerOptionalProperty(BreakableBehaviour, m_useDestroySound, m_destroySound, "Sound");
        setPropertyDescription(BreakableBehaviour, m_destroySound, "Name of the sound to play on destroy (a Sound component is required)");

        registerOptionalProperty(BreakableBehaviour, m_useImpulse, m_impulse, "Impulse");
        setPropertyDescription(BreakableBehaviour, m_impulse, "The strength of the physics impulse when object is destroyed");
    }
    registerPropertyGroupEnd(BreakableBehaviour);

    registerPropertyEnumEx(BreakableBehaviour, BreakableState, m_breakableState, "State", vg::core::PropertyFlags::Transient);
    setPropertyDescription(BreakableBehaviour, m_breakableState, "Chest state");

    return true;
}

//--------------------------------------------------------------------------------------
void BreakableBehaviour::OnPlay()
{
    super::OnPlay();

    // Reset chest state (as it's not serialized so it's not restored on restart)
    m_breakableState = BreakableState::Default;
}

//--------------------------------------------------------------------------------------
void BreakableBehaviour::Update(const Context & _context)
{
    if (_context.m_playing && !_context.m_paused && !_context.m_world->IsPrefabWorld())
    {
        if (m_useRotation)
        {
            IGameObject * go = GetGameObject();
            float4x4 matrix = go->GetLocalMatrix();
            float4x4 rotZ = float4x4::rotation_z(m_rotation.z * _context.m_dt);
            matrix = mul(matrix, rotZ);
            go->SetLocalMatrix(matrix);
        }
    }
}

//--------------------------------------------------------------------------------------
void BreakableBehaviour::OnTriggerEnter(vg::core::IGameObject * _other)
{
    if (CharacterBehaviour * attacker = _other->GetComponentT<CharacterBehaviour>())
        TakeHit(attacker, nullptr);
}

//--------------------------------------------------------------------------------------
bool BreakableBehaviour::TakeHit(CharacterBehaviour * _attacker, ItemBehaviour * _weapon)
{
    if (m_breakableState == BreakableState::Default)
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

            m_breakableState = BreakableState::Destroyed;

            if (_attacker)
            {
                if (m_giveLifes > 0)
                    _attacker->addLifes(m_giveLifes);

                if (m_giveHP)
                    _attacker->addHP(m_giveHP);

                if (m_giveScore)
                    _attacker->addScore(m_giveScore);
            }

            //if (m_useDestroyAnim)
            //{
            //    if (auto * animComp = thisGameObject->GetComponentT<IAnimationComponent>())
            //    {
            //        uint animIndex = animComp->GetAnimationIndex(m_destroyAnimName);
            //        if (-1 != animIndex)
            //            animComp->PlayAnimation(animIndex, false);
            //        else
            //            VG_WARNING("[Chest] Missing 'Destroyed' animation \"%s\"", m_destroyAnimName.c_str());
            //    }
            //    else
            //    {
            //        VG_WARNING("[Chest] Missing IAnimationComponent");
            //    }
            //}

            if (m_useDestroySound)
            {
                if (auto * soundComponent = GetGameObject()->GetComponentT<ISoundComponent>())
                {
                    uint soundIndex = soundComponent->GetSoundIndex(m_destroySound);
                    if (-1 != soundIndex)
                        soundComponent->Play(soundIndex);
                    else
                        VG_WARNING("[Chest] Missing 'Destroyed' sound \"%s\"", m_destroySound.c_str());
                }
                else
                {
                    VG_WARNING("[Chest] Missing ISoundComponent");
                }
            }

            //// Disable physics
            //if (auto * physicsBodyComp = thisGameObject->GetComponentT<IPhysicsBodyComponent>())
            //    physicsBodyComp->Enable(false);

            // Disable original object
            if (IGameObject * defaultGameObject = m_default.get<vg::core::IGameObject>())
            {
                defaultGameObject->Enable(false);

                // Show destroyed objects
                if (IGameObject * destroyedGameObject = m_destroyed.get<IGameObject>())
                {
                    destroyedGameObject->SetLocalMatrix(defaultGameObject->GetLocalMatrix());
                    destroyedGameObject->Enable(true);

                    if (m_useImpulse)
                    {
                        auto & children = destroyedGameObject->GetChildren();
                        for (uint i = 0; i < children.size(); ++i)
                        {
                            IGameObject * part = children[i];

                            if (part->isEnabledInHierarchy())
                            {
                                if (IPhysicsBodyComponent * physicsBodyComp = part->GetComponentT<IPhysicsBodyComponent>())
                                {
                                    float rx = Random::getRandomInRange(-m_impulse.x, +m_impulse.x);
                                    float ry = Random::getRandomInRange(-m_impulse.y, +m_impulse.y);
                                    float rz = m_impulse.z;

                                    physicsBodyComp->AddImpulse(float3(rx, ry, rz));
                                }
                            }
                        }
                    }
                }
            }

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



