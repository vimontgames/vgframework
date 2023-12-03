#include "engine/Precomp.h"
#include "RigidBodyComponent.h"
#include "core/IGameObject.h"

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
        _desc.registerPropertyEnum(RigidBodyComponent, physics::Shape, m_shape, "Shape");

        return true;
    }

    //--------------------------------------------------------------------------------------
    RigidBodyComponent::RigidBodyComponent(const core::string & _name, IObject * _parent) :
        super(_name, _parent)
    {
        if (getGameObject())
            updateFlagsFromGameObject();
    }

    //--------------------------------------------------------------------------------------
    RigidBodyComponent::~RigidBodyComponent()
    {

    }

    //--------------------------------------------------------------------------------------
    void RigidBodyComponent::Update(double _dt)
    {

    }
    
    //--------------------------------------------------------------------------------------
    void RigidBodyComponent::OnPlay()
    {

    }

    //--------------------------------------------------------------------------------------
    void RigidBodyComponent::OnStop()
    {

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

        super::OnPropertyChanged(_object, _prop, _notifyParent);
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