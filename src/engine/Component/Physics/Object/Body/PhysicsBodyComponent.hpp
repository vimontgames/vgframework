#include "PhysicsBodyComponent.h"
#include "engine/Component/Physics/Shape/PhysicsShapeComponent.h"
#include "engine/EngineOptions.h"
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
                updateFlagsFromGameObject();

            if (m_body == nullptr)
                createBody();
        }
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

            if (_context.m_world->IsPlaying() && !_context.m_world->IsPaused())   
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

            const auto * engineOptions = EngineOptions::get();
            if (engineOptions->isAnyBodyVisible())
            {
                const auto shapes = GetGameObject()->GetComponentsT<PhysicsShapeComponent>();
                if (shapes.size() > 0)
                {
                    float totalMass = 0.0f;
                    for (uint i = 0; i < shapes.size(); ++i)
                    {
                        const auto * shape = shapes[i];
                        if (shape && shape->isEnabled())
                        {
                            const auto * physicsShape = shape->getPhysicsShape();
                            if (engineOptions->isBodyVisible(physicsShape->GetShapeType()))
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

        if (m_body)
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
        else if (!strcmp(_prop.GetName(), "m_flags"))
        {
            auto * go = dynamic_cast<IGameObject *>(_object);
            if (go)
            {
                VG_ASSERT(go == GetGameObject());
                if (updateFlagsFromGameObject())
                    VG_INFO("[PhysicsBody] Updated flags because GameObject flags changed");
            }
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
    bool PhysicsBodyComponent::createBody()
    {
        IWorld * world = GetGameObject()->GetWorld();

        if (nullptr == world || false == world->IsPlaying())
            return false;
 
        VG_SAFE_RELEASE(m_body);

        const auto shapes = GetGameObject()->GetComponentsT<PhysicsShapeComponent>();
        vector<physics::ShapeInfo> physicsShapes;
        float totalMass = 0.0f;
        for (uint i = 0; i < shapes.size(); ++i)
        {
            auto & shape = shapes[i];
            if (shape->isEnabled())
            {
                if (physics::IShape * physicsShape = shape->getPhysicsShape())
                {
                    physics::ShapeInfo & info = physicsShapes.emplace_back();
                    info.m_shape = physicsShape;
                    totalMass += physicsShape->GetMass();
                }
            }
        }

        if (physicsShapes.size() > 0)
        {
            m_bodyDesc->SetMass(totalMass);

            if (m_bodyDesc->GetMotionType() != physics::MotionType::Static || !EngineOptions::get()->mergeStaticBodies())
                m_body = getPhysics()->CreateBody(world->GetPhysicsWorld(), m_bodyDesc, physicsShapes, GetGameObject()->GetGlobalMatrix(), GetGameObject()->GetName() + "_PhysicsBody", this);
        }
        else
        {
            VG_WARNING("[Physics] PhysicsBodyComponent in GameObject \"%s\" has no PhysicsShapeComponent", GetGameObject()->GetName().c_str());
        }

        updateShapesColor();

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
                return 0xFF0000FF;
            else
                return 0xFF00FFFF; 
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