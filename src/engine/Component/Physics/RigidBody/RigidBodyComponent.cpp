#include "engine/Precomp.h"
#include "RigidBodyComponent.h"
#include "core/IGameObject.h"
#include "engine/Engine.h"
#include "renderer/IDisplayOptions.h"
#include "physics/IShape.h"
#include "physics/IPhysics.h"
#include "physics/IBody.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(RigidBodyComponent, "RigidBody Component");

    //--------------------------------------------------------------------------------------
    bool RigidBodyComponent::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyObjectPtrEx(RigidBodyComponent, m_bodyDesc, "Body", IProperty::Flags::Flatten);
        registerPropertyObjectPtrEx(RigidBodyComponent, m_shapeDesc, "Shape", IProperty::Flags::Flatten);

        return true;
    }

    //--------------------------------------------------------------------------------------
    RigidBodyComponent::RigidBodyComponent(const core::string & _name, IObject * _parent) :
        super(_name, _parent)
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

    //--------------------------------------------------------------------------------------
    RigidBodyComponent::~RigidBodyComponent()
    {
        VG_SAFE_RELEASE(m_body);
        VG_SAFE_RELEASE(m_shape);
        VG_SAFE_RELEASE(m_bodyDesc);
        VG_SAFE_RELEASE(m_shapeDesc);
    }

    //--------------------------------------------------------------------------------------
    void RigidBodyComponent::Update(double _dt)
    {
        const auto * engine = Engine::get();
        IGameObject * go = GetGameObject();

        if (physics::MotionType::Static != m_bodyDesc->GetMotion())
        {
            if (engine->IsPlaying() && !engine->isPaused())
            {
                if (m_body)
                {
                    float4x4 world = m_body->GetMatrix();
                    go->setWorldMatrix(world);
                }
            }
        }
        
        if (engine->getDisplayOptions()->IsCollidersEnabled())
        {
            if (m_shape)
                m_shape->Draw(go->getWorldMatrix());
        }
    }

    //--------------------------------------------------------------------------------------
    void RigidBodyComponent::OnLoad()
    {
        if (m_shapeDesc)
            createShape();
        if (m_shape)
            createBody();
    }
    
    //--------------------------------------------------------------------------------------
    void RigidBodyComponent::OnPlay()
    {
        if (m_body)
            m_body->Activate(GetGameObject()->GetWorldMatrix());

        if (m_shapeDesc)
            m_shapeDesc->OnPlay();

        super::OnPlay();
    }

    //--------------------------------------------------------------------------------------
    void RigidBodyComponent::OnStop()
    {
        super::OnStop();

        if (m_shapeDesc)
            m_shapeDesc->OnStop();

        if (m_body)
            m_body->Deactivate(GetGameObject()->GetWorldMatrix());
    }
      
    //--------------------------------------------------------------------------------------
    void RigidBodyComponent::OnPropertyChanged(IObject * _object, const core::IProperty & _prop, bool _notifyParent)
    {
        if (!strcmp(_prop.getName(), "m_flags"))
        {
            auto * go = dynamic_cast<IGameObject *>(_object);
            if (go)
            {
                VG_ASSERT(go == GetGameObject());
                if (updateFlagsFromGameObject())
                    VG_INFO("[RigidBody] Updated RigidBody because GameObject flags changed");
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
    bool RigidBodyComponent::createBodyDesc()
    {
        IFactory * factory = Kernel::getFactory();
        VG_SAFE_RELEASE(m_bodyDesc);

        m_bodyDesc = (physics::IBodyDesc *)factory->createObject("RigidBodyDesc", "", this);

        return nullptr != m_bodyDesc;
    }

    //--------------------------------------------------------------------------------------
    bool RigidBodyComponent::createShapeDesc()
    {
        IFactory * factory = Kernel::getFactory();
        VG_SAFE_RELEASE(m_shapeDesc);

        VG_ASSERT(m_bodyDesc);
        const physics::ShapeType shapeType = m_bodyDesc->GetShapeType();
        switch (shapeType)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(shapeType);
                break;
        
            case physics::ShapeType::Sphere:
                m_shapeDesc = (physics::IShapeDesc*)factory->createObject("SphereShapeDesc", "", this);
                break;
        
            case physics::ShapeType::Box:
                m_shapeDesc = (physics::IShapeDesc *)factory->createObject("BoxShapeDesc", "", this);
                break;
        }

        return nullptr != m_shapeDesc;
    }

    //--------------------------------------------------------------------------------------
    physics::IPhysics * RigidBodyComponent::getPhysics()
    {
        return Engine::get()->GetPhysics();
    }

    //--------------------------------------------------------------------------------------
    bool RigidBodyComponent::createShape()
    {
        VG_SAFE_RELEASE(m_shape);
        VG_ASSERT(m_shapeDesc);
        if (m_shapeDesc)
            m_shape = getPhysics()->CreateShape(m_shapeDesc);
        return nullptr != m_shape;
    }

    //--------------------------------------------------------------------------------------
    bool RigidBodyComponent::createBody()
    {
        VG_SAFE_RELEASE(m_body);
        VG_ASSERT(m_bodyDesc && m_shape);
        if (m_bodyDesc && m_shape)
        {                
            m_body = getPhysics()->CreateBody(m_bodyDesc, m_shape, GetGameObject()->GetWorldMatrix());
        
            if (!m_bodyDesc->IsMassOverriden())
                m_bodyDesc->SetMass(m_shape->GetMass());
        }
        return nullptr != m_body;
    }

    //--------------------------------------------------------------------------------------
    // Called at creation or when GameObject flags change. 
    // Updates the layer and motion flags according to Static/Dynamic GameObject
    //--------------------------------------------------------------------------------------
    bool RigidBodyComponent::updateFlagsFromGameObject()
    {
        IGameObject * go = GetGameObject();
        VG_ASSERT(go);
        if (nullptr != go)
        {
            // Update static/dynamic physics flags
            if (asBool(IInstance::Flags::Static & go->getFlags()))
            {
                if (physics::ObjectLayer::NonMoving != m_bodyDesc->GetLayer() || physics::MotionType::Static != m_bodyDesc->GetMotion())
                {
                    m_bodyDesc->SetLayer(physics::ObjectLayer::NonMoving);
                    m_bodyDesc->SetMotion(physics::MotionType::Static);
                    return true;
                }
            }
            else
            {
                if (physics::ObjectLayer::Moving != m_bodyDesc->GetLayer() || physics::MotionType::Dynamic != m_bodyDesc->GetMotion())
                {
                    m_bodyDesc->SetLayer(physics::ObjectLayer::Moving);
                    m_bodyDesc->SetMotion(physics::MotionType::Dynamic);
                    return true;
                }
            }
        }
        return false;
    }
      
    //--------------------------------------------------------------------------------------
    void RigidBodyComponent::onResourceLoaded(core::IResource * _resource)
    {

    }

    //--------------------------------------------------------------------------------------
    void RigidBodyComponent::onResourceUnloaded(core::IResource * _resource)
    {

    }
}