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
    VG_REGISTER_COMPONENT_CLASS(CharacterControllerComponent, "Character Controller", "Physics", "Cheap and accurate collisions between the character and dynamic bodies.", editor::style::icon::CharacterController, getPriority(ComponentGroup::Physics, ComponentPriority::Early, ComponentMultithreadType::Job));

    //--------------------------------------------------------------------------------------
    bool CharacterControllerComponent::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEnumEx(CharacterControllerComponent, physics::CharacterType, m_characterType, "Type", PropertyFlags::ReadOnly);
        registerPropertyObjectPtrEx(CharacterControllerComponent, m_characterDesc, "Character", PropertyFlags::Flatten);
        registerPropertyObjectPtrEx(CharacterControllerComponent, m_shapeDesc, "Shape", PropertyFlags::Flatten);

        
        registerPropertyEnumEx(CharacterControllerComponent, physics::GroundState, m_groundState, "State", PropertyFlags::Transient);
        registerPropertyEnumBitfield(CharacterControllerComponent, Tag, m_groundStateExcludeTags, "Exclude Tags");
        setPropertyDescription(CharacterControllerComponent, m_groundStateExcludeTags, "Objetcs with these tags will be ignored to 'Grounded' state transition");

        return true;
    }

    //--------------------------------------------------------------------------------------
    CharacterControllerComponent::CharacterControllerComponent(const core::string & _name, IObject * _parent) :
        super(_name, _parent)
    {
        if (m_shapeDesc == nullptr)
            createShapeDesc();

        if (m_shape == nullptr)
            createShape();

        if (m_characterDesc == nullptr)
            createCharacterDesc();

        if (m_character == nullptr)
            createCharacter();

        EnableUpdateFlags(UpdateFlags::Update | UpdateFlags::ToolUpdate);
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
        m_shapeDesc = (physics::IShapeDesc *)factory->CreateObject("CapsuleShapeDesc", "", this);
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

            m_characterDesc = (physics::ICharacterDesc *)factory->CreateObject("RigidCharacterDesc", "", this);

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
                m_character = getPhysics()->CreateCharacter(world->GetPhysicsWorld(), m_characterDesc, m_shape, GetGameObject()->GetGlobalMatrix(), GetGameObject()->GetName() + "_CharacterController", this);
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
        super::OnPlay();
    }

    //--------------------------------------------------------------------------------------
    void CharacterControllerComponent::OnStop()
    {
        super::OnStop();        
    }

    //--------------------------------------------------------------------------------------
    void CharacterControllerComponent::OnEnable()
    {
        super::OnEnable();

        if (!m_character)
            createCharacter();

        if (m_character)
            m_character->Activate(GetGameObject()->GetGlobalMatrix());

        if (m_shapeDesc)
            m_shapeDesc->OnEnable();

        if (m_characterDesc)
            m_characterDesc->OnEnable();
    }

    //--------------------------------------------------------------------------------------
    void CharacterControllerComponent::OnDisable()
    {
        super::OnDisable();

        // Instead of detroying the character on 'Disable' we should make it asleep
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
            if (_context.m_playing && !_context.m_paused)
            {
                m_character->Update();

                float4x4 matrix = m_character->GetMatrix();
                _context.m_gameObject->setGlobalMatrix(matrix);

                auto groundState = m_character->GetGroundState();
                const IComponent * groundComponent = dynamic_cast<const IComponent*>(m_character->GetGroundObject());

                if (nullptr != groundComponent && asBool(m_groundStateExcludeTags & groundComponent->GetGameObject()->GetTags()) && groundState == physics::GroundState::Grounded)
                    m_groundState = physics::GroundState::InAir;
                else
                    m_groundState = groundState;
            }
        }  
    } 

    //--------------------------------------------------------------------------------------
    void CharacterControllerComponent::ToolUpdate(const Context & _context)
    {
        if (Engine::get()->GetPhysics()->GetOptions()->IsBodyVisible(m_shape->GetShapeType(), physics::MotionType::Kinematic))
        {
            if (m_shape)
                m_shape->Draw(_context.m_world, _context.m_gameObject->getGlobalMatrix());
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

    //--------------------------------------------------------------------------------------
    bool CharacterControllerComponent::CanTeleportTo(const core::float3 & _position)
    {
        VG_ASSERT(m_character, "[Physics] 'CanTeleportTo' requires a valid character controller shape");
        if (m_character)
            return m_character->CanTeleportTo(_position);
        return true;
    }
}