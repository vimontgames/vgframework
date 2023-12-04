#pragma once

#include "physics/IShape.h"

namespace vg::physics
{
    class BodyDesc : public IBodyDesc
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(BodyDesc, IBodyDesc);

        virtual BodyType GetBodyType() const = 0;

        ShapeType GetShapeType() const final override { return m_shapeType; }

        ObjectLayer GetLayer() const final override { return m_layer; }
        void SetLayer(ObjectLayer _layer) final override { m_layer = _layer; }

        MotionType GetMotion() const final override { return m_motion; }
        void SetMotion(MotionType _motion)  final override { m_motion = _motion; }

        bool IsMassOverriden() const final override { return m_overrideMass; }
        float GetMass() const final override { return m_mass; }
        void SetMass(float _mass) final override { m_mass = _mass; }

    protected:
        renderer::IDebugDraw * getDebugDraw() const;

    public:
        ObjectLayer m_layer         = ObjectLayer::NonMoving;
        MotionType  m_motion        = MotionType::Static;
        ShapeType   m_shapeType     = ShapeType::Box;
        bool        m_overrideMass  = false;
        float       m_mass          = 0.0f;
    };

    class RigidBodyDesc final : public BodyDesc
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(RigidBodyDesc, BodyDesc);
        BodyType GetBodyType() const final override { return BodyType::Rigid; }
    };
}