#pragma once

#include "core/Object/Object.h"
#include "Physics_Consts.h"

namespace vg::physics
{
    class IBodyDesc : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IBodyDesc, core::Object)

        virtual bool                IsTrigger       () const = 0;
        virtual bool                IsSoftBody      () const = 0;

        virtual BodyType            GetBodyType     () const = 0;
        virtual ShapeType           GetShapeType    () const = 0;

        virtual ObjectLayer         GetLayer        () const = 0;
        virtual void                SetLayer        (ObjectLayer _layer) = 0;

        virtual MotionType          GetMotion       () const = 0;
        virtual void                SetMotion       (MotionType _motion) = 0;

        virtual bool                IsMassOverriden () const = 0;
        virtual float               GetMass         () const = 0;
        virtual void                SetMass         (float _mass) = 0;
    };

    class IBody : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IBody, core::Object)

        virtual void Activate(const core::float4x4 & _world) = 0;
        virtual void Deactivate(const core::float4x4 & _world) = 0;

        virtual core::float4x4 GetMatrix() const = 0;
    };
}