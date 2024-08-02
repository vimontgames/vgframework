#include "PhysicsBodyComponent.h"
#include "core/IGameObject.h"
#include "core/IWorld.h"
#include "engine/Engine.h"
#include "physics/IPhysicsOptions.h"
#include "physics/IShape.h"
#include "physics/IPhysics.h"
#include "physics/IBody.h"
#include "editor/Editor_Consts.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(PhysicsBodyComponent, "Physics Body", "Physics", "A physics body to interact with physics world", editor::style::icon::PhysicsBody, 0);

    //--------------------------------------------------------------------------------------
    bool PhysicsBodyComponent::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        //registerPropertyEnumEx(PhysicsBodyComponent, physics::BodyType, m_bodyType, "Type", IProperty::Flags::ReadOnly);
        registerPropertyObjectPtrEx(PhysicsBodyComponent, m_bodyDesc, "Body", IProperty::Flags::Flatten);
        registerPropertyObjectPtrEx(PhysicsBodyComponent, m_shapeDesc, "Shape", IProperty::Flags::Flatten);

        return true;
    }

    //--------------------------------------------------------------------------------------
    PhysicsBodyComponent::PhysicsBodyComponent(const core::string & _name, IObject * _parent) :
        super(_name, _parent)
    {
        if (_parent)
        {
            if (m_bodyDesc == nullptr)
                createBodyDesc();

            if (m_shapeDesc == nullptr)
                createShapeDesc();

            if (getGameObject())
                updateFlagsFromGameObject();

            if (m_shape == nullptr)
                createShape();

            if (m_body == nullptr)
                createBody();
        }
    }

    //--------------------------------------------------------------------------------------
    PhysicsBodyComponent::~PhysicsBodyComponent()
    {
        VG_SAFE_RELEASE(m_body);
        VG_SAFE_RELEASE(m_shape);
        VG_SAFE_RELEASE(m_bodyDesc);
        VG_SAFE_RELEASE(m_shapeDesc);
    }

    //--------------------------------------------------------------------------------------
    void PhysicsBodyComponent::Update(float _dt)
    {
        if (nullptr == m_bodyDesc)
            return;
        
        IGameObject * go = GetGameObject(); 
        auto * world = go->GetWorld();
        if (nullptr == world)
            return;        

        if (physics::MotionType::Static != m_bodyDesc->GetMotion() && !m_bodyDesc->IsTrigger())
        {
            if (world->IsPlaying() && !world->IsPaused())
            {
                if (m_body)
                {
                    auto matrix = m_body->GetMatrix();
                    go->setGlobalMatrix(matrix);
                }
            }
        }

        const auto * engine = Engine::get();
        if (engine->getPhysicsOptions()->IsBodyVisible(m_shape->GetShapeType()))
        {
            if (m_shape)
                m_shape->Draw(world, go->getGlobalMatrix());
        }
    }

    //--------------------------------------------------------------------------------------
    void PhysicsBodyComponent::OnLoad()
    {
        createShape();
        
        if (m_shapeDesc)
            m_shapeDesc->RegisterUID();

        createBody();

        if (m_bodyDesc)
            m_bodyDesc->RegisterUID();
    }

    //--------------------------------------------------------------------------------------
    void PhysicsBodyComponent::OnEnable()
    {
        if (!m_body)
            createBody();

        if (m_body)
            m_body->Activate(GetGameObject()->GetGlobalMatrix());

        if (m_shapeDesc)
            m_shapeDesc->OnEnable();

        if (m_bodyDesc)
            m_bodyDesc->OnEnable();

        super::OnEnable();
    }

    //--------------------------------------------------------------------------------------
    void PhysicsBodyComponent::OnDisable()
    {
        super::OnDisable();

        VG_SAFE_RELEASE(m_body);

        if (m_shapeDesc)
            m_shapeDesc->OnDisable();

        if (m_bodyDesc)
            m_bodyDesc->OnStop();

        if (m_body)
            m_body->Deactivate(GetGameObject()->GetGlobalMatrix());
    }

    //--------------------------------------------------------------------------------------
    void PhysicsBodyComponent::OnPropertyChanged(IObject * _object, const core::IProperty & _prop, bool _notifyParent)
    {
        if (!strcmp(_prop.getName(), "m_trigger"))
        {
            createBody();
            createShape();
        }
        else if (!strcmp(_prop.getName(), "m_bodyType"))
        {
            createBodyDesc();
            createBody();
            createShapeDesc();
            createShape();
        }
        else if (!strcmp(_prop.getName(), "m_flags"))
        {
            auto * go = dynamic_cast<IGameObject *>(_object);
            if (go)
            {
                VG_ASSERT(go == GetGameObject());
                if (updateFlagsFromGameObject())
                    VG_INFO("[PhysicsBody] Updated flags because GameObject flags changed");
            }
        }
        else if (!strcmp(_prop.getName(), "m_shapeType"))
        {
            if (nullptr == m_shapeDesc || m_bodyDesc->GetShapeType() != m_shapeDesc->GetShapeType())
            {
                createShapeDesc();
                createShape();
                createBody();
            }
        }
        else if (_object == m_shapeDesc)
        {
            createShape();
            createBody();
        }
        else if (_object == m_bodyDesc)
        {
            createBody();
        }

        super::OnPropertyChanged(_object, _prop, _notifyParent);
    }

    //--------------------------------------------------------------------------------------
    bool PhysicsBodyComponent::createBodyDesc()
    {
        if (nullptr == m_bodyDesc /* || m_bodyType != m_bodyDesc->GetBodyType()*/)
        {
            UID originalUID = 0;
            if (m_bodyDesc)
                originalUID = m_bodyDesc->GetOriginalUID(false);

            IFactory * factory = Kernel::getFactory();
            VG_SAFE_RELEASE(m_bodyDesc);

            m_bodyDesc = (physics::IBodyDesc *)factory->createObject("PhysicsBodyDesc", "", this);

            if (m_bodyDesc)
            {
                m_bodyDesc->setParent(this);
                m_bodyDesc->RegisterUID();
                if (originalUID)
                    m_bodyDesc->SetOriginalUID(originalUID);
            }
        }

        return nullptr != m_bodyDesc;
    }

    //--------------------------------------------------------------------------------------
    bool PhysicsBodyComponent::createShapeDesc()
    {
        IFactory * factory = Kernel::getFactory();

        //UID previousUID = 0;
        UID previousOriginalUID = 0;
        if (nullptr != m_shapeDesc)
        {
            //previousUID = m_shapeDesc->GetUID(false);
            previousOriginalUID = m_shapeDesc->GetOriginalUID(false);
        }

        VG_SAFE_RELEASE(m_shapeDesc);

        VG_ASSERT(m_bodyDesc);
        if (m_bodyDesc)
        {
            const physics::ShapeType shapeType = m_bodyDesc->GetShapeType();
            switch (shapeType)
            {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(shapeType);
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
            }
        }

        if (m_shapeDesc)
        {
            //if (previousUID)
            //    m_shapeDesc->SetUID(previousUID);
            //
            //m_shapeDesc->RegisterUID();

            if (previousOriginalUID)
                m_shapeDesc->SetOriginalUID(previousOriginalUID);
        }

        return nullptr != m_shapeDesc;
    }

    //--------------------------------------------------------------------------------------
    physics::IPhysics * PhysicsBodyComponent::getPhysics()
    {
        return Engine::get()->GetPhysics();
    }

    //--------------------------------------------------------------------------------------
    bool PhysicsBodyComponent::createShape()
    {
        VG_SAFE_RELEASE(m_shape);
        VG_ASSERT(m_shapeDesc);
        if (m_shapeDesc)
            m_shape = getPhysics()->CreateShape(m_shapeDesc);

        if (m_shape)
        {
            if (m_bodyDesc->IsTrigger())
            {
                m_shape->SetColor(0xFF7F00FF);
            }
            else
            {
                if (m_bodyDesc->GetMotion() == physics::MotionType::Dynamic)
                    m_shape->SetColor(0xFF0000FF);
                else
                    m_shape->SetColor(0xFF00FFFF);
            }
        }

        return nullptr != m_shape;
    }

    //--------------------------------------------------------------------------------------
    bool PhysicsBodyComponent::createBody()
    {
        if (IWorld * world = GetGameObject()->GetWorld())
        {
            if (!world->IsPlaying())
                return false;
        }
        else
        {
            return false;
        }

        VG_SAFE_RELEASE(m_body);
        VG_ASSERT(m_bodyDesc && m_shape);
        if (m_bodyDesc && m_shape)
        {
            if (!m_bodyDesc->IsMassOverriden())
                m_bodyDesc->SetMass(m_shape->GetMass());

            if (auto * world = GetGameObject()->GetWorld())
                m_body = getPhysics()->CreateBody(world->GetPhysicsWorld(), m_bodyDesc, m_shape, GetGameObject()->GetGlobalMatrix());
        }
        return nullptr != m_body;
    }

    //--------------------------------------------------------------------------------------
    // Called at creation or when GameObject flags change. 
    // Updates the layer and motion flags according to Static/Dynamic GameObject
    // It will recreate the physics body if needed.
    //--------------------------------------------------------------------------------------
    bool PhysicsBodyComponent::updateFlagsFromGameObject()
    {
        IGameObject * go = GetGameObject();
        VG_ASSERT(go);
        bool updated = false;
        if (nullptr != go)
        {
            // Update static/dynamic physics flags
            if (asBool(InstanceFlags::Static & go->getInstanceFlags()))
            {
                if (physics::ObjectLayer::NonMoving != m_bodyDesc->GetLayer() || physics::MotionType::Static != m_bodyDesc->GetMotion())
                {
                    m_bodyDesc->SetLayer(physics::ObjectLayer::NonMoving);
                    m_bodyDesc->SetMotion(physics::MotionType::Static);
                    updated = true;
                }
            }
            else
            {
                if (physics::ObjectLayer::Moving != m_bodyDesc->GetLayer() || physics::MotionType::Dynamic != m_bodyDesc->GetMotion())
                {
                    m_bodyDesc->SetLayer(physics::ObjectLayer::Moving);
                    m_bodyDesc->SetMotion(physics::MotionType::Dynamic);
                    updated = true;
                }
            }
        }

        if (updated)
        {
            if (m_body)
            {
                VG_SAFE_RELEASE(m_body);
                createBody();
            }
        }

        return updated;
    }

    //--------------------------------------------------------------------------------------
    void PhysicsBodyComponent::onResourceLoaded(core::IResource * _resource)
    {

    }

    //--------------------------------------------------------------------------------------
    void PhysicsBodyComponent::onResourceUnloaded(core::IResource * _resource)
    {

    }

    //--------------------------------------------------------------------------------------
    bool PhysicsBodyComponent::TryGetAABB(core::AABB & _aabb) const
    {
        if (m_shapeDesc)
        {
            if (m_shapeDesc->TryGetAABB(_aabb))
                return true;
        }

        return false;
    }
}