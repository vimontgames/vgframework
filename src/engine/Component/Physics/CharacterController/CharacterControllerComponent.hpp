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
    VG_REGISTER_COMPONENT_CLASS(CharacterControllerComponent, "Character Controller", "Physics", "Cheap and accurate collisions between the character and dynamic bodies.");

    //--------------------------------------------------------------------------------------
    bool CharacterControllerComponent::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEnumEx(CharacterControllerComponent, physics::CharacterType, m_characterType, "Type", IProperty::Flags::ReadOnly);
        registerPropertyObjectPtrEx(CharacterControllerComponent, m_characterDesc, "Character", IProperty::Flags::Flatten);
        registerPropertyObjectPtrEx(CharacterControllerComponent, m_shapeDesc, "Shape", IProperty::Flags::Flatten);

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
            m_character = getPhysics()->CreateCharacter(m_characterDesc, m_shape, GetGameObject()->GetWorldMatrix());

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
        if (m_character)
            m_character->Activate(GetGameObject()->GetWorldMatrix());

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

        if (m_shapeDesc)
            m_shapeDesc->OnStop();

        if (m_characterDesc)
            m_characterDesc->OnStop();

        if (m_character)
            m_character->Deactivate(GetGameObject()->GetWorldMatrix());
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
    void CharacterControllerComponent::Update(float _dt)
    {
        const auto * engine = Engine::get();
        IGameObject * go = GetGameObject();

        //if (physics::CharacterType::Virtual != m_characterDesc->GetCharacterType())
        {
            if (engine->isPlaying() && !engine->isPaused())
            {
                if (m_character)
                {
                    float4x4 world = m_character->GetMatrix();
                    go->setWorldMatrix(world);
                }
            }
        }

        if (engine->getPhysicsOptions()->IsRigidBodyVisible(m_shape->GetShapeType()))
        {
            if (m_shape)
                m_shape->Draw(go->getWorldMatrix());
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
}