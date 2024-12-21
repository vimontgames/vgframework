#pragma once

#include "engine/IPhysicsBodyComponent.h"
#include "physics/Physics_Consts.h"

namespace vg::physics
{
    class IPhysics;
    class IBody;
    class IBodyDesc;
}

namespace vg::engine
{
    class PhysicsShapeComponent;

    class PhysicsBodyComponent final : public IPhysicsBodyComponent
    {
    public:
        VG_CLASS_DECL(PhysicsBodyComponent, IPhysicsBodyComponent);

        PhysicsBodyComponent(const core::string & _name, IObject * _parent);
        ~PhysicsBodyComponent();

        void                                    Update              (const Context & _context) final override;
        void                                    OnLoad              () final override;
        void                                    OnPlay              () final override;
        void                                    OnStop              () final override;
        void                                    OnPropertyChanged   (core::IObject * _object, const core::IProperty & _prop, bool _notifyParent) final override;
        bool                                    TryGetAABB          (core::AABB & _aabb) const final override;

        void                                    SetMatrix           (const core::float4x4 & _matrix) final override;
        void                                    AddImpulse          (const core::float3 & _impulse) final override;

        void                                    SetTrigger          (bool _trigger) final override;
        bool                                    IsTrigger           () const final override;

        void                                    SetMotionType       (physics::MotionType _motionType) final override;
        physics::MotionType                     GetMotionType       () const final override;

        const core::float3 &                    GetVelocity         () const final override;
        
        void                                    onShapeUpdated      ();
        void                                    updateShapesColor   ();

        core::vector<PhysicsShapeComponent *>   getShapes           () const;
        core::u32                               getShapesColor      () const;
        const physics::IBodyDesc *              getBodyDesc         () const { return m_bodyDesc; }

    private:
        bool                                    updateFlagsFromGameObject();

        bool                                    createBodyDesc();
        bool                                    createBody();

        static physics::IPhysics *              getPhysics();

    private:
        physics::IBodyDesc *                    m_bodyDesc  = nullptr;
        physics::IBody *                        m_body      = nullptr;
        core::float3                            m_velocity  = (core::float3)0.0f;
    };
}