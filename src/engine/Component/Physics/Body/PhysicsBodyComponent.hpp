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
        if (physics::MotionType::Static != m_bodyDesc->GetMotion())
        {
            if (nullptr == _context.m_world)
                return;

            if (nullptr == m_bodyDesc)
                return;

            if (_context.m_world->IsPlaying() && !_context.m_world->IsPaused())   // TODO: use context?
            {
                if (m_body)
                {
                    if (m_bodyDesc->IsTrigger())
                    {
                        m_body->SetMatrix(_context.m_gameObject->getGlobalMatrix());
                    }
                    else
                    {
                        auto matrix = m_body->GetMatrix();
                        _context.m_gameObject->setGlobalMatrix(matrix);
                    }
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

            m_bodyDesc = (physics::IBodyDesc *)factory->createObject("PhysicsBodyDesc", "", this);

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

            if (m_bodyDesc->GetMotion() != physics::MotionType::Static || !EngineOptions::get()->mergeStaticBodies())
                m_body = getPhysics()->CreateBody(world->GetPhysicsWorld(), m_bodyDesc, physicsShapes, GetGameObject()->GetGlobalMatrix(), GetGameObject()->getName() + "_PhysicsBody", this);
        }
        else
        {
            VG_WARNING("[Physics] PhysicsBodyComponent in GameObject \"%s\" has no PhysicsShapeComponent", GetGameObject()->getName().c_str());
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
            if (m_bodyDesc->GetMotion() == physics::MotionType::Dynamic)
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
}