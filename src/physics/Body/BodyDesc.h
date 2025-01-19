#pragma once

#include "physics/IShape.h"

namespace vg::physics
{
    class PhysicsBodyDesc : public IBodyDesc
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(PhysicsBodyDesc, IBodyDesc);

        void                SetTrigger          (bool _trigger)         { m_trigger = _trigger; }
        bool                IsTrigger           () const final override { return m_trigger; }

        bool                IsSoftBody          () const final override { return m_softBody;}

        BodyType            GetBodyType         () const final override { return m_softBody ? BodyType::Soft : BodyType::Rigid; }

        ObjectLayer         GetLayer            () const final override { return m_layer; }
        void                SetLayer            (ObjectLayer _layer) final override { m_layer = _layer; }

        MotionType          GetMotionType           () const final override { return m_motion; }
        void                SetMotionType           (MotionType _motion)  final override { m_motion = _motion; }

        MotionQuality       GetMotionQuality    () const final override { return m_motionQuality; }
        void                SetMotionQuality    (MotionQuality _quality) { m_motionQuality = _quality; }

        bool                IsMassOverriden     () const final override { return m_overrideMass; }
        float               GetMass             () const final override { return m_mass; }
        void                SetMass             (float _mass) final override { m_overrideMass = true; m_mass = _mass; }

    public:
        bool                m_trigger           = false;
        bool                m_softBody          = false;
        ObjectLayer         m_layer             = ObjectLayer::NonMoving;
        MotionType          m_motion            = MotionType::Static;
        MotionQuality       m_motionQuality     = MotionQuality::Discrete;
        bool                m_overrideMass      = false;
        float               m_mass              = 0.0f;
        float               m_friction          = 0.5f;
        float               m_restitution       = 0.0f;
    };

}