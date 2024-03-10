#pragma once

#include "engine/IRigidBodyComponent.h"
#include "physics/Physics_Consts.h"

namespace vg::physics
{
    class IPhysics;
    class IShape;
    class IShapeDesc;
    class IBody;
    class IBodyDesc;
}

namespace vg::engine
{
    class RigidBodyComponent final : public IRigidBodyComponent
    {
    public:
        VG_CLASS_DECL(RigidBodyComponent, IRigidBodyComponent);

        RigidBodyComponent(const core::string & _name, IObject * _parent);
        ~RigidBodyComponent();

        void                        Update(float _dt) final override;

        void                        OnLoad() final override;

        void                        OnPlay() final override;
        void                        OnStop() final override;

        void                        OnPropertyChanged(core::IObject * _object, const core::IProperty & _prop, bool _notifyParent) final override;

        void                        onResourceLoaded(core::IResource * _resource) final override;
        void                        onResourceUnloaded(core::IResource * _resource) final override;

        bool                        TryGetAABB(core::AABB & _aabb) const final override;

    private:
        bool                        updateFlagsFromGameObject();

        bool                        createBodyDesc();
        bool                        createBody();

        bool                        createShapeDesc();
        bool                        createShape();

        static physics::IPhysics *  getPhysics();

    private:
        physics::IBodyDesc *        m_bodyDesc  = nullptr;
        physics::IBody *            m_body      = nullptr;
        physics::IShapeDesc *       m_shapeDesc = nullptr;  
        physics::IShape *           m_shape     = nullptr;
    };
}