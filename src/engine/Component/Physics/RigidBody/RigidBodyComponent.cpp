#include "engine/Precomp.h"
#include "RigidBodyComponent.h"
#include "core/IGameObject.h"
#include "engine/Engine.h"
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

        _desc.registerPropertyEnumEx(RigidBodyComponent, physics::ObjectLayer, m_layer, "Layer", IProperty::Flags::ReadOnly);
        _desc.registerPropertyEnumEx(RigidBodyComponent, physics::MotionType, m_motion, "Motion", IProperty::Flags::ReadOnly);
        _desc.registerPropertyEnum(RigidBodyComponent, physics::ShapeType, m_shapeType, "Shape");

        _desc.registerPropertyObjectPtrEx(RigidBodyComponent, m_shapeDesc, "Shape Settings", IProperty::Flags::Flatten);

        return true;
    }

    //--------------------------------------------------------------------------------------
    RigidBodyComponent::RigidBodyComponent(const core::string & _name, IObject * _parent) :
        super(_name, _parent)
    {
        if (getGameObject())
            updateFlagsFromGameObject();

        if (m_shapeDesc == nullptr)
            createShapeDesc();
    }

    //--------------------------------------------------------------------------------------
    RigidBodyComponent::~RigidBodyComponent()
    {
        VG_SAFE_RELEASE(m_shapeDesc);
        VG_SAFE_RELEASE(m_shape);
        VG_SAFE_RELEASE(m_body);
    }

    //--------------------------------------------------------------------------------------
    void RigidBodyComponent::Update(double _dt)
    {
        IGameObject * go = GetGameObject();

        if (physics::MotionType::Static != m_motion)
        {
            if (Engine::get()->IsPlaying())
            {
                if (m_body)
                {
                    float4x4 world = m_body->GetMatrix();
                    go->setWorldMatrix(world);
                }
            }
        }

        if (m_shape)
            m_shapeDesc->Draw(go->getWorldMatrix());
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

        super::OnPlay();
    }

    //--------------------------------------------------------------------------------------
    void RigidBodyComponent::OnStop()
    {
        super::OnStop();

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
            if (nullptr == m_shapeDesc || m_shapeType != m_shapeDesc->GetShapeType())
                createShapeDesc();
        }
        else if (_object == m_shapeDesc)
        {
            createShape();
            createBody();
        }

        super::OnPropertyChanged(_object, _prop, _notifyParent);
    }

    //--------------------------------------------------------------------------------------
    bool RigidBodyComponent::createShapeDesc()
    {
        IFactory * factory = Kernel::getFactory();
        VG_SAFE_RELEASE(m_shapeDesc);

        switch (m_shapeType)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(m_shapeType);
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
        VG_ASSERT(m_shape);
        if (m_shape)
            m_body = getPhysics()->CreateBody(m_shape, GetGameObject()->GetWorldMatrix(), m_motion, m_layer);
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
                if (physics::ObjectLayer::NonMoving != m_layer || physics::MotionType::Static != m_motion)
                {
                    m_layer = physics::ObjectLayer::NonMoving;
                    m_motion = physics::MotionType::Static;
                    return true;
                }
            }
            else
            {
                if (physics::ObjectLayer::Moving != m_layer || physics::MotionType::Dynamic != m_motion)
                {
                    m_layer = physics::ObjectLayer::Moving;
                    m_motion = physics::MotionType::Dynamic;
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