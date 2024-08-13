#include "PhysicsShapeComponent.h"
#include "core/IGameObject.h"
#include "core/IWorld.h"
#include "engine/Engine.h"
#include "physics/IPhysicsOptions.h"
#include "physics/IShape.h"
#include "editor/Editor_Consts.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(PhysicsShapeComponent, "Physics Shape", "Physics", "A shape to use with physics body", editor::style::icon::PhysicsShape, 0);

    //--------------------------------------------------------------------------------------
    bool PhysicsShapeComponent::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEnumEx(PhysicsShapeComponent, physics::ShapeType, m_shapeType, "Type", IProperty::Flags::None);
        setPropertyDescription(PhysicsShapeComponent, m_shapeType, "Type of primitive to use for the physics shape");

        registerPropertyObjectPtrEx(PhysicsShapeComponent, m_shapeDesc, "Shape", IProperty::Flags::Flatten);

        return true;
    }

    //--------------------------------------------------------------------------------------
    PhysicsShapeComponent::PhysicsShapeComponent(const core::string & _name, IObject * _parent) :
        super(_name, _parent)
    {
        SetUpdateFlags(UpdateFlags::Update, false);

        if (_parent)
        {
            if (m_shapeDesc == nullptr)
                createShapeDesc();

            if (m_shape == nullptr)
                createShape();
        }
    }

    //--------------------------------------------------------------------------------------
    PhysicsShapeComponent::~PhysicsShapeComponent()
    {
        VG_SAFE_RELEASE(m_shape);
        VG_SAFE_RELEASE(m_shapeDesc);
    }

    //--------------------------------------------------------------------------------------
    PhysicsBodyComponent * PhysicsShapeComponent::getBodyComponent()
    {
        return GetGameObject()->GetComponentByType<PhysicsBodyComponent>();
    }

    //--------------------------------------------------------------------------------------
    void PhysicsShapeComponent::OnLoad()
    {
        createShape();

        if (m_shapeDesc)
            m_shapeDesc->RegisterUID();
    }

    //--------------------------------------------------------------------------------------
    void PhysicsShapeComponent::Update(float _dt)
    {
        if (isEnabled())
        {
            auto shape = getPhysicsShape();

            IGameObject * go = GetGameObject();
            auto * world = go->GetWorld();
            if (nullptr == world)
                return;

            if (shape)
            {
                const auto & physicsOptions = Engine::get()->getPhysicsOptions();
                if (physicsOptions->IsBodyVisible(shape->GetShapeType()))
                    shape->Draw(world, go->getGlobalMatrix());
            }
            else
            {
                VG_WARNING("[Physics] PhysicsShapeComponent in GameObject \"%s\" has no physics shape", go->getName().c_str());
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void PhysicsShapeComponent::OnPropertyChanged(IObject * _object, const core::IProperty & _prop, bool _notifyParent)
    {
        if (!strcmp(_prop.getName(), "m_shapeType") || strstr(_prop.GetClassName(), "ShapeDesc"))
        {
            createShapeDesc();
            createShape();
        }

        if (auto * bodyComp = getBodyComponent())
            bodyComp->onShapeUpdated();

        super::OnPropertyChanged(_object, _prop, _notifyParent);
    }

    //--------------------------------------------------------------------------------------
    bool PhysicsShapeComponent::createShapeDesc()
    {
        IFactory * factory = Kernel::getFactory();

        if (nullptr == m_shapeDesc || m_shapeDesc->GetShapeType() != m_shapeType)
        {
            UID previousOriginalUID = 0;
            if (nullptr != m_shapeDesc)
                previousOriginalUID = m_shapeDesc->GetOriginalUID(false);

            VG_SAFE_RELEASE(m_shapeDesc);

            switch (m_shapeType)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(m_shapeType);
                    break;

                case physics::ShapeType::Sphere:
                    m_shapeDesc = (physics::IShapeDesc *)factory->createObject("SphereShapeDesc", "", this);
                    break;

                case physics::ShapeType::Box:
                    m_shapeDesc = (physics::IShapeDesc *)factory->createObject("BoxShapeDesc", "", this);
                    break;

                case physics::ShapeType::Capsule:
                    m_shapeDesc = (physics::IShapeDesc *)factory->createObject("CapsuleShapeDesc", "", this);
                    break;

                case physics::ShapeType::Cylinder:
                    m_shapeDesc = (physics::IShapeDesc *)factory->createObject("CylinderShapeDesc", "", this);
                    break;
            }

            if (m_shapeDesc)
            {
                VG_ASSERT(m_shapeDesc->GetShapeType() == m_shapeType);

                if (previousOriginalUID)
                    m_shapeDesc->SetOriginalUID(previousOriginalUID);
            }
        }

        return nullptr != m_shapeDesc;
    }

    //--------------------------------------------------------------------------------------
    physics::IPhysics * PhysicsShapeComponent::getPhysics()
    {
        return Engine::get()->GetPhysics();
    }

    //--------------------------------------------------------------------------------------
    bool PhysicsShapeComponent::createShape()
    {
        VG_SAFE_RELEASE(m_shape);
        VG_ASSERT(m_shapeDesc);
        if (m_shapeDesc)
            m_shape = getPhysics()->CreateShape(m_shapeDesc);
        
        if (auto * bodyComp = getBodyComponent())
        {
            m_shape->SetColor(bodyComp->getShapesColor());
        }

        return nullptr != m_shape;
    }

    //--------------------------------------------------------------------------------------
    bool PhysicsShapeComponent::TryGetAABB(core::AABB & _aabb) const
    {
        if (m_shapeDesc)
        {
            if (m_shapeDesc->TryGetAABB(_aabb))
                return true;
        }

        return false;
    }
}