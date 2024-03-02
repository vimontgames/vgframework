#include "CharacterControllerComponent.h"
#include "core/IGameObject.h"
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

        registerPropertyGroupBegin(CharacterControllerComponent, "Debug");
        registerPropertyEnumEx(CharacterControllerComponent, physics::GroundState, m_groundState, "State", IProperty::Flags::NotSaved);
        registerPropertyGroupEnd(CharacterControllerComponent);

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

        return nullptr != m_shapeDesc;
    }

    //--------------------------------------------------------------------------------------
    bool CharacterControllerComponent::createShape()
    {
        VG_SAFE_RELEASE(m_shape);
        VG_ASSERT(m_shapeDesc);
        if (m_shapeDesc)
            m_shape = getPhysics()->CreateShape(m_shapeDesc);
        return nullptr != m_shape;
    }

    //--------------------------------------------------------------------------------------
    bool CharacterControllerComponent::createCharacterDesc()
    {
        IFactory * factory = Kernel::getFactory();
        VG_SAFE_RELEASE(m_characterDesc);

        m_characterDesc = (physics::ICharacterDesc *)factory->createObject("RigidCharacterDesc", "", this);
        
        return nullptr != m_characterDesc;
    }

    //--------------------------------------------------------------------------------------
    bool CharacterControllerComponent::createCharacter()
    {
        VG_SAFE_RELEASE(m_character);
        VG_ASSERT(m_characterDesc && m_shape);
        if (m_characterDesc && m_shape)
        {
            if (auto * world = GetGameObject()->GetWorld())
                m_character = getPhysics()->CreateCharacter(world->GetPhysicsWorld(), m_characterDesc, m_shape, GetGameObject()->GetGlobalMatrix());
        }
        return nullptr != m_character;
    }

    //--------------------------------------------------------------------------------------
    void CharacterControllerComponent::OnLoad()
    {
        if (m_shapeDesc)
            createShape();
        if (m_shape)
            createCharacter();
    }

    //--------------------------------------------------------------------------------------
    void CharacterControllerComponent::OnPlay()
    {
        if (!m_character)
            createCharacter();

        if (m_character)
            m_character->Activate(GetGameObject()->GetGlobalMatrix());

        if (m_shapeDesc)
            m_shapeDesc->OnPlay();

        if (m_characterDesc)
            m_characterDesc->OnPlay();

        super::OnPlay();
    }

    //--------------------------------------------------------------------------------------
    void CharacterControllerComponent::OnStop()
    {
        super::OnStop();

        VG_SAFE_RELEASE(m_character);

        if (m_shapeDesc)
            m_shapeDesc->OnStop();

        if (m_characterDesc)
            m_characterDesc->OnStop();

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
    void CharacterControllerComponent::FixedUpdate(float _dt)
    {
        const auto * engine = Engine::get();
        IGameObject * go = GetGameObject();

        if (engine->getPhysicsOptions()->IsRigidBodyVisible(m_shape->GetShapeType()))
        {
            if (m_shape)
                m_shape->Draw(go->GetWorld(), go->getGlobalMatrix());
        }
    }

    //--------------------------------------------------------------------------------------
    physics::GroundState CharacterControllerComponent::GetGroundState() const
    {
        return m_groundState;
    }

    //--------------------------------------------------------------------------------------
    void CharacterControllerComponent::Update(float _dt)
    {
        const auto * engine = Engine::get();
        IGameObject * go = GetGameObject();

        if (m_character)
        {
            auto * world = GetGameObject()->GetWorld();

            if (world->IsPlaying() && !world->IsPaused())
            {
                m_character->Update();
                m_groundState = m_character->GetGroundState();

                float4x4 matrix = m_character->GetMatrix();
                go->setGlobalMatrix(matrix);
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
}