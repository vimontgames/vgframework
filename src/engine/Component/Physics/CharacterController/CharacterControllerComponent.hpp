#include "CharacterControllerComponent.h"
#include "core/IGameObject.h"
#include "core/Math/Math.h"
#include "engine/Engine.h"
#include "physics/IPhysicsOptions.h"
#include "physics/IShape.h"
#include "physics/IPhysics.h"
#include "physics/ICharacter.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(CharacterControllerComponent, "Character Controller", "Physics", "Cheap and accurate collisions between the character and dynamic bodies.", editor::style::icon::CharacterController, -1);

    //--------------------------------------------------------------------------------------
    bool CharacterControllerComponent::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEnumEx(CharacterControllerComponent, physics::CharacterType, m_characterType, "Type", IProperty::Flags::ReadOnly);
        registerPropertyObjectPtrEx(CharacterControllerComponent, m_characterDesc, "Character", IProperty::Flags::Flatten);
        registerPropertyObjectPtrEx(CharacterControllerComponent, m_shapeDesc, "Shape", IProperty::Flags::Flatten);
        registerPropertyEnumArrayEx(CharacterControllerComponent, u8, physics::GroundState, m_delayState, "GroundState Delay", IProperty::Flags::Flatten);

        registerPropertyGroupBegin(CharacterControllerComponent, "Debug");
        registerPropertyEnumEx(CharacterControllerComponent, physics::GroundState, m_groundState, "State", IProperty::Flags::NotSaved);
        registerPropertyGroupEnd(CharacterControllerComponent);

        return true;
    }

    //--------------------------------------------------------------------------------------
    CharacterControllerComponent::CharacterControllerComponent(const core::string & _name, IObject * _parent) :
        super(_name, _parent)
    {
        for (uint i = 0; i < core::enumCount<physics::GroundState>(); ++i)
        {
            m_delayState[i] = 0;
            m_delayStateCounter[i] = -1;
        }

        if (m_shapeDesc == nullptr)
            createShapeDesc();

        if (m_shape == nullptr)
            createShape();

        if (m_characterDesc == nullptr)
            createCharacterDesc();

        if (m_character == nullptr)
            createCharacter();

        SetUpdateFlags(UpdateFlags::FixedUpdate | UpdateFlags::Update);
    }

    //--------------------------------------------------------------------------------------
    CharacterControllerComponent::~CharacterControllerComponent()
    {
        VG_SAFE_RELEASE(m_shapeDesc);
        VG_SAFE_RELEASE(m_shape);
        VG_SAFE_RELEASE(m_characterDesc);
        VG_SAFE_RELEASE(m_character);      
    }

    //--------------------------------------------------------------------------------------
    physics::IPhysics * CharacterControllerComponent::getPhysics()
    {
        return Engine::get()->GetPhysics();
    }

    //--------------------------------------------------------------------------------------
    bool CharacterControllerComponent::createShapeDesc()
    {
        IFactory * factory = Kernel::getFactory();
        VG_SAFE_RELEASE(m_shapeDesc);
        m_shapeDesc = (physics::IShapeDesc *)factory->createObject("CapsuleShapeDesc", "", this);
        m_shapeDesc->SetParent(this);

        return nullptr != m_shapeDesc;
    }

    //--------------------------------------------------------------------------------------
    bool CharacterControllerComponent::createShape()
    {
        VG_SAFE_RELEASE(m_shape);
        VG_ASSERT(m_shapeDesc);
        if (m_shapeDesc)
            m_shape = getPhysics()->CreateShape(m_shapeDesc);

        if (m_shape)
            m_shape->SetColor(0xFFAFAFAF);

        return nullptr != m_shape;
    }

    //--------------------------------------------------------------------------------------
    bool CharacterControllerComponent::createCharacterDesc()
    {
        if (nullptr == m_characterDesc)
        {
            IFactory * factory = Kernel::getFactory();

            m_characterDesc = (physics::ICharacterDesc *)factory->createObject("RigidCharacterDesc", "", this);

            if (m_characterDesc)
            {
                m_characterDesc->SetParent(this);
                m_characterDesc->RegisterUID();
            }
        }
        
        return nullptr != m_characterDesc;
    }

    //--------------------------------------------------------------------------------------
    bool CharacterControllerComponent::createCharacter()
    {
        VG_SAFE_RELEASE(m_character);

        if (auto * world = GetGameObject()->GetWorld())
        {
            if (nullptr == world || false == world->IsPlaying())
                return false;

            VG_ASSERT(m_characterDesc && m_shape);
            if (m_characterDesc && m_shape)
                m_character = getPhysics()->CreateCharacter(world->GetPhysicsWorld(), m_characterDesc, m_shape, GetGameObject()->GetGlobalMatrix(), GetGameObject()->getName() + "_CharacterController", this);
        }
        return nullptr != m_character;
    }

    //--------------------------------------------------------------------------------------
    void CharacterControllerComponent::OnLoad()
    {
        if (m_shapeDesc)
            createShape();

        if (m_shapeDesc)
            m_shapeDesc->RegisterUID();

        if (m_shape)
            createCharacter();

        if (m_shape)
            m_shape->RegisterUID();
    }

    //--------------------------------------------------------------------------------------
    void CharacterControllerComponent::OnPlay()
    {
        if (!m_character)
            createCharacter();

        if (m_character)
            m_character->Activate(GetGameObject()->GetGlobalMatrix());

        if (m_shapeDesc)
            m_shapeDesc->OnEnable();

        if (m_characterDesc)
            m_characterDesc->OnEnable();

        super::OnPlay();
    }

    //--------------------------------------------------------------------------------------
    void CharacterControllerComponent::OnStop()
    {
        super::OnStop();

        VG_SAFE_RELEASE(m_character);

        if (m_shapeDesc)
            m_shapeDesc->OnDisable();

        if (m_characterDesc)
            m_characterDesc->OnDisable();

        if (m_character)
            m_character->Deactivate(GetGameObject()->GetGlobalMatrix());
    }

    //--------------------------------------------------------------------------------------
    void CharacterControllerComponent::OnPropertyChanged(IObject * _object, const core::IProperty & _prop, bool _notifyParent)
    {
        if (_object == this)
        {
            createShape();
            createCharacterDesc();
            createCharacter();
        }
        else if (_object == m_shapeDesc)
        {
            createShape();
            createCharacter();
        }
        else if (_object == m_characterDesc)
        {
            createCharacter();
        }

        super::OnPropertyChanged(_object, _prop, _notifyParent);
    }

    //--------------------------------------------------------------------------------------
    void CharacterControllerComponent::FixedUpdate(const Context & _context)
    {
        if (EngineOptions::get()->isBodyVisible(m_shape->GetShapeType()))
        {
            if (m_shape)
                m_shape->Draw(_context.m_world, _context.m_gameObject->getGlobalMatrix());
        }
    }

    //--------------------------------------------------------------------------------------
    physics::GroundState CharacterControllerComponent::GetGroundState() const
    {
        return m_groundState;
    }

    //--------------------------------------------------------------------------------------
    void CharacterControllerComponent::Update(const Context & _context)
    {
        const auto * engine = Engine::get();

        if (m_character)
        {
            if (_context.m_world->IsPlaying() && !_context.m_world->IsPaused())
            {
                m_character->Update();

                auto newGroundState = m_character->GetGroundState();

                if (m_groundState != newGroundState)
                {
                    const auto index = asInteger(newGroundState);
                    const auto delay = m_delayState[index];
                    if (0 != delay)
                    {
                        auto & counter = m_delayStateCounter[index];

                        if (0xFF == counter)
                        {
                            counter = 0;
                        }
                        else
                        {
                            counter = min(++counter, delay);
                        }

                        if (counter >= delay)
                        {
                            m_groundState = newGroundState;
                            counter = -1;
                        }
                    }
                    else
                    {
                        m_groundState = newGroundState;
                    }
                }

                float4x4 matrix = m_character->GetMatrix();
                _context.m_gameObject->setGlobalMatrix(matrix);
            }
        }  
    } 

    //--------------------------------------------------------------------------------------
    core::float3 CharacterControllerComponent::GetVelocity() const
    {
        if (m_character)
            return m_character->GetVelocity(); 
        else
            return float3(0, 0, 0);
    }

    //--------------------------------------------------------------------------------------
    void CharacterControllerComponent::SetVelocity(const core::float3 & _velocity)
    {
        if (m_character)
            return m_character->SetVelocity(_velocity);
    }

    //--------------------------------------------------------------------------------------
    void CharacterControllerComponent::SetPosition(const core::float3 & _position)
    {
        if (m_character)
            return m_character->SetPosition(_position);
    }

    //--------------------------------------------------------------------------------------
    void CharacterControllerComponent::SetRotation(const core::quaternion & _rotation)
    {
        if (m_character)
            return m_character->SetRotation(_rotation);
    }

    //--------------------------------------------------------------------------------------
    bool CharacterControllerComponent::TryGetAABB(core::AABB & _aabb) const
    {
        if (m_shapeDesc)
        {
            if (m_shapeDesc->TryGetAABB(_aabb))
                return true;
        }

        return false;
    }
}