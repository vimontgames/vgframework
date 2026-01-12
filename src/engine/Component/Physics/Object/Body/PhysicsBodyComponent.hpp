#include "PhysicsBodyComponent.h"
#include "engine/Component/Physics/Shape/PhysicsShapeComponent.h"
#include "engine/EngineOptions.h"
#include "engine/IVehicleComponent.h"
#include "core/IGameObject.h"
#include "core/IWorld.h"
#include "core/Misc/AABB/AABB.h"
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

        registerPropertyObjectPtrEx(PhysicsBodyComponent, m_bodyDesc, "Body", PropertyFlags::Flatten);

        registerPropertyGroupBegin(PhysicsBodyComponent, "Velocity");
        {
            registerPropertyEx(PhysicsBodyComponent, m_linearVelocity, "Linear", PropertyFlags::Transient | PropertyFlags::ReadOnly);
            registerPropertyEx(PhysicsBodyComponent, m_angularVelocity, "Angular", PropertyFlags::Transient | PropertyFlags::ReadOnly);
        }
        registerPropertyGroupEnd(PhysicsBodyComponent);

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

            if (getGameObject())
                UpdateFlagsFromGameObject();

            if (m_body == nullptr)
                createBody();
        }

        EnableUpdateFlags(UpdateFlags::Update | UpdateFlags::ToolUpdate, true);
    }

    //--------------------------------------------------------------------------------------
    PhysicsBodyComponent::~PhysicsBodyComponent()
    {
        VG_SAFE_RELEASE(m_body);
        VG_SAFE_RELEASE(m_bodyDesc);
    }

    //--------------------------------------------------------------------------------------
    void PhysicsBodyComponent::Update(const Context & _context)
    {
        if (physics::MotionType::Static != m_bodyDesc->GetMotionType())
        {
            if (nullptr == _context.m_world)
                return;

            if (nullptr == m_bodyDesc)
                return;

            if (_context.m_playing && !_context.m_paused)
            {
                if (m_body)
                {
                    if (m_bodyDesc->IsTrigger() || m_bodyDesc->GetMotionType() == physics::MotionType::Kinematic)
                    {
                        SetMatrix(_context.m_gameObject->getGlobalMatrix());
                    }
                    else
                    {
                        auto matrix = m_body->GetMatrix();
                        _context.m_gameObject->setGlobalMatrix(matrix);
                    }
                }
            }            
        }

        if (m_body)
        {
            m_linearVelocity = m_body->GetLinearVelocity();
            m_angularVelocity = m_body->GetAngularVelocity();
        }
        else
        {
            m_linearVelocity = (core::float3)0.0f;
            m_angularVelocity = (core::float3)0.0f;
        }
    }

    //--------------------------------------------------------------------------------------
    void PhysicsBodyComponent::ToolUpdate(const Context & _context)
    {
        const auto * physicsOptions = Engine::get()->GetPhysics()->GetOptions();
        if (physicsOptions->IsAnyBodyVisible())
        {
            const auto shapes = GetGameObject()->GetComponentsT<PhysicsShapeComponent>();
            if (shapes.size() > 0)
            {
                for (uint i = 0; i < shapes.size(); ++i)
                {
                    const auto * shape = shapes[i];
                    if (shape && shape->isEnabled())
                    {
                        const auto * physicsShape = shape->getPhysicsShape();
                        if (physicsShape && physicsOptions->IsBodyVisible(physicsShape->GetShapeType(), m_bodyDesc->GetMotionType()))
                            physicsShape->Draw(_context.m_world, _context.m_gameObject->getGlobalMatrix());
                    }
                }
            }
            else
            {
                VG_WARNING("[Physics] PhysicsBodyComponent in GameObject \"%s\" has no physics shape", _context.m_gameObject->GetName().c_str());
            }
        }
    }

    //--------------------------------------------------------------------------------------
    bool PhysicsBodyComponent::CanStaticMerge() const
    {
        if (m_bodyDesc)
            return m_bodyDesc->CanStaticMerge();
        return false;
    }

    //--------------------------------------------------------------------------------------
    void PhysicsBodyComponent::SetLinearVelocity(const core::float3 & _velocity)
    {
        if (m_body)
            m_body->SetLinearVelocity(_velocity);
        m_linearVelocity = _velocity;
    }

    //--------------------------------------------------------------------------------------
    void PhysicsBodyComponent::SetAngularVelocity(const core::float3 & _velocity)
    {
        if (m_body)
            m_body->SetAngularVelocity(_velocity);
        m_angularVelocity = _velocity;
    }

    //--------------------------------------------------------------------------------------
    const core::float3 & PhysicsBodyComponent::GetAngularVelocity() const
    {
        return m_angularVelocity;
    }

    //--------------------------------------------------------------------------------------
    const core::float3 & PhysicsBodyComponent::GetLinearVelocity() const
    {
        return m_linearVelocity;
    }

    //--------------------------------------------------------------------------------------
    void PhysicsBodyComponent::OnLoad()
    {
        createBody();

        if (m_bodyDesc)
            m_bodyDesc->RegisterUID();
    }

    //--------------------------------------------------------------------------------------
    void PhysicsBodyComponent::OnPlay()
    {
        if (!m_body)
            createBody();

        // If a vehicule component is present, don't start immediatly
        IComponent * contraintComp = nullptr; // GetGameObject()->GetComponentByType("VehicleComponent");

        if (m_body && !contraintComp)
            m_body->Activate(GetGameObject()->GetGlobalMatrix());

        if (m_bodyDesc)
            m_bodyDesc->OnPlay();

        // Static do not need runtime update
        //if (physics::MotionType::Static != m_bodyDesc->GetMotion())
        //    SetUpdateFlags(UpdateFlags::Update, true);
        //else
        //    SetUpdateFlags(UpdateFlags::Update, false);

        super::OnLoad();
    }

    //--------------------------------------------------------------------------------------
    void PhysicsBodyComponent::OnStop()
    {
        super::OnStop();

        // Restore
        //SetUpdateFlags(UpdateFlags::Update, true);

        VG_SAFE_RELEASE(m_body);

        if (m_bodyDesc)
            m_bodyDesc->OnStop();

        if (m_body)
            m_body->Deactivate(GetGameObject()->GetGlobalMatrix());
    }

    //--------------------------------------------------------------------------------------
    void PhysicsBodyComponent::SetTrigger(bool _trigger)
    {
        if (m_bodyDesc)
        {
            if (m_bodyDesc->IsTrigger() != _trigger)
            {
                m_bodyDesc->SetTrigger(_trigger);
                createBody();
            }
        }   
    }

    //--------------------------------------------------------------------------------------
    bool PhysicsBodyComponent::IsTrigger() const
    {
        if (m_bodyDesc)
            return m_bodyDesc->IsTrigger();
        return false;
    }

    //--------------------------------------------------------------------------------------
    void PhysicsBodyComponent::SetMotionType(physics::MotionType _motionType)
    {
        if (m_bodyDesc)
        {
            if (m_bodyDesc->GetMotionType() != _motionType)
            {
                m_bodyDesc->SetMotionType(_motionType);
                createBody();
            }
        }
    }

    //--------------------------------------------------------------------------------------
    physics::MotionType PhysicsBodyComponent::GetMotionType() const
    {
        if (m_bodyDesc)
            return m_bodyDesc->GetMotionType();
        return (physics::MotionType)-1;
    }

    //--------------------------------------------------------------------------------------
    void PhysicsBodyComponent::OnPropertyChanged(IObject * _object, const core::IProperty & _prop, bool _notifyParent)
    {
        if (!strcmp(_prop.GetName(), "m_trigger"))
        {
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
        if (nullptr == m_bodyDesc)
        {
             UID originalUID = 0;
            if (m_bodyDesc)
                originalUID = m_bodyDesc->GetOriginalUID(false);

            IFactory * factory = Kernel::getFactory();
            VG_SAFE_RELEASE(m_bodyDesc);

            m_bodyDesc = (physics::IBodyDesc *)factory->CreateObject("PhysicsBodyDesc", "", this);

            if (m_bodyDesc)
            {
                m_bodyDesc->SetParent(this);
                m_bodyDesc->RegisterUID();
                if (originalUID)
                    m_bodyDesc->SetOriginalUID(originalUID);
            }
        }

        return nullptr != m_bodyDesc;
    }

    //--------------------------------------------------------------------------------------
    physics::IPhysics * PhysicsBodyComponent::getPhysics()
    {
        return Engine::get()->GetPhysics();
    }

    //--------------------------------------------------------------------------------------
    void PhysicsBodyComponent::OnEnable()
    {
        super::OnEnable();

        if (!m_body)
            createBody();
    }

    //--------------------------------------------------------------------------------------
    void PhysicsBodyComponent::OnDisable()
    {
        super::OnDisable();

        // Also disable vehicle component that depends on this body
        //if (IVehicleComponent * vehicleComp = GetGameObject()->GetComponentT<IVehicleComponent>())
        //    vehicleComp->Enable(false);

        VG_SAFE_RELEASE(m_body);
    }

    //--------------------------------------------------------------------------------------
    bool PhysicsBodyComponent::createBody()
    {
        IGameObject * go = GetGameObject();
        
        // body desc and gameobject's instance flags should match
        if (m_bodyDesc->GetMotionType() == physics::MotionType::Static && !asBool(InstanceFlags::Static & go->GetInstanceFlags()))
            VG_WARNING("[Physics] Physics component from gameobject \"%s\" uses 'MotionType::Static' but gameobject is not Static", go->GetName().c_str());
        else if (m_bodyDesc->GetMotionType() == physics::MotionType::Dynamic && asBool(InstanceFlags::Static & go->GetInstanceFlags()))
            VG_WARNING("[Physics] Physics component from gameobject \"%s\" uses 'MotionType::Dynamic' but gameobject is Static", go->GetName().c_str());

        IWorld * world = go->GetWorld();

        const auto shapes = go->GetComponentsT<PhysicsShapeComponent>();
        vector<physics::ShapeInfo> physicsShapes;

        updateShapesColor();

        if (nullptr == world || false == world->IsPlaying())
            return false;

        VG_SAFE_RELEASE(m_body);

        float totalMass = 0.0f;
        for (uint i = 0; i < shapes.size(); ++i)
        {
            auto & shape = shapes[i];
            if (shape->isEnabled())
            {
                if (physics::IShape * physicsShape = shape->getPhysicsShape())
                {
                    if (physicsShape->GetShapeType() == physics::ShapeType::Mesh)
                    {
                        if (this->GetMotionType() != physics::MotionType::Static)
                        {
                            VG_WARNING("[Physics] Mesh colliders are only supported for static colliders");
                            continue;
                        }
                    }

                    physics::ShapeInfo & info = physicsShapes.emplace_back();
                    info.m_shape = physicsShape;
                    totalMass += physicsShape->GetMass();
                }
            }
        }

        if (totalMass <= 0.0f)
        {
            VG_WARNING("[Physics] Body from GameObject \"%s\" cannot have a total mass of %f. Defaulting mass to 1.0f.", go->GetName().c_str(), totalMass);
            m_bodyDesc->SetMass(1.0f);
        }

        if (physicsShapes.size() > 0)
        {
            if (!m_bodyDesc->IsMassOverriden())
                m_bodyDesc->SetMass(totalMass);

            if (m_bodyDesc->GetMotionType() != physics::MotionType::Static || !Engine::get()->GetPhysics()->GetOptions()->IsMergeStaticBodiesEnabled() || !m_bodyDesc->CanStaticMerge())
                m_body = getPhysics()->CreateBody(world->GetPhysicsWorld(), m_bodyDesc, physicsShapes, go->GetGlobalMatrix(), go->GetName() + "_PhysicsBody", this);
        }
        else
        {
            VG_WARNING("[Physics] PhysicsBodyComponent in GameObject \"%s\" has no PhysicsShapeComponent", GetGameObject()->GetName().c_str());
        }

        return nullptr != m_body;
    }

    //--------------------------------------------------------------------------------------
    // Called at creation or when GameObject flags change. 
    // Updates the layer and motion flags according to Static/Dynamic GameObject
    // It will recreate the physics body if needed.
    //--------------------------------------------------------------------------------------
    bool PhysicsBodyComponent::UpdateFlagsFromGameObject()
    {
        super::UpdateFlagsFromGameObject();

        IGameObject * go = GetGameObject();
        VG_ASSERT(go);
        bool updated = false;
        if (nullptr != go)
        {
            // Update static/dynamic physics flags
            if (asBool(InstanceFlags::Static & go->getInstanceFlags()))
            {
                if (physics::ObjectLayer::NonMoving != m_bodyDesc->GetLayer() || physics::MotionType::Static != m_bodyDesc->GetMotionType())
                {
                    m_bodyDesc->SetLayer(physics::ObjectLayer::NonMoving);
                    m_bodyDesc->SetMotionType(physics::MotionType::Static);
                    updated = true;
                }
            }
            else
            {
                if (physics::ObjectLayer::Moving != m_bodyDesc->GetLayer() || physics::MotionType::Dynamic != m_bodyDesc->GetMotionType())
                {
                    m_bodyDesc->SetLayer(physics::ObjectLayer::Moving);
                    m_bodyDesc->SetMotionType(physics::MotionType::Dynamic);
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
    // Shape has been update, need to rebuild physics body
    //--------------------------------------------------------------------------------------
    void PhysicsBodyComponent::onShapeUpdated()
    {
        createBody();
    }

    //--------------------------------------------------------------------------------------
    core::u32 PhysicsBodyComponent::getShapesColor() const
    {
        if (m_bodyDesc->IsTrigger())
        {
            return 0xFF7F00FF;
        }
        else
        {
            if (m_bodyDesc->GetMotionType() == physics::MotionType::Dynamic)
            {
                return 0xFF0000FF;
            }
            else
            {
                const auto & engine = Engine::get();
                if (m_bodyDesc->CanStaticMerge() && engine->IsPlaying() && engine->getPhysicsOptions()->IsMergeStaticBodiesEnabled())
                    return 0xFFFFFFFF;
                else
                    return 0xFF00FFFF;
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void PhysicsBodyComponent::updateShapesColor()
    {
        const auto & shapes = getShapes();
        for (uint i = 0; i < shapes.size(); ++i)
        {
            if (auto * shape = shapes[i])
            {
                if (auto * physicsShape = shape->getPhysicsShape())
                    physicsShape->SetColor(getShapesColor());
            }
        }
    }

    //--------------------------------------------------------------------------------------
    vector<PhysicsShapeComponent *> PhysicsBodyComponent::getShapes() const
    {
        return GetGameObject()->GetComponentsT<PhysicsShapeComponent>();
    }

    //--------------------------------------------------------------------------------------
    bool PhysicsBodyComponent::TryGetAABB(core::AABB & _aabb) const
    {
        bool found = false;
        AABB compAABB;

        const auto & shapes = getShapes();
        for (uint i = 0; i < shapes.size(); ++i)
        {
            AABB shapeAABB;
            if (shapes[i]->TryGetAABB(shapeAABB))
            {
                compAABB.grow(shapeAABB);
                found = true;
            }
        }

        return found;
    }

    //--------------------------------------------------------------------------------------
    void PhysicsBodyComponent::AddImpulse(const core::float3 & _impulse)
    {
        if (m_body)
            m_body->AddImpulse(_impulse);
    }

    //--------------------------------------------------------------------------------------
    void PhysicsBodyComponent::SetMatrix(const core::float4x4 & _matrix)
    {
        m_body->SetMatrix(_matrix);
    }
}