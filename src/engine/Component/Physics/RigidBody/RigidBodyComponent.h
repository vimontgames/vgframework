#pragma once

#include "engine/IRigidBodyComponent.h"
#include "physics/Physics_Consts.h"

namespace vg::engine
{
    class RigidBodyComponent final : public IRigidBodyComponent
    {
    public:
        VG_CLASS_DECL(RigidBodyComponent, IRigidBodyComponent);

        RigidBodyComponent(const core::string & _name, IObject * _parent);
        ~RigidBodyComponent();

        void                    Update(double _dt) final override;

        void                    OnPlay() final override;
        void                    OnStop() final override;

        void                    OnPropertyChanged(core::IObject * _object, const core::IProperty & _prop, bool _notifyParent) final override;

        void                    onResourceLoaded(core::IResource * _resource) final override;
        void                    onResourceUnloaded(core::IResource * _resource) final override;

    private:
        bool                    updateFlagsFromGameObject();

    private:
        physics::ObjectLayer    m_layer = physics::ObjectLayer::NonMoving;
        physics::MotionType     m_motion = physics::MotionType::Static;
        physics::Shape          m_shape = physics::Shape::Sphere;
    };
}