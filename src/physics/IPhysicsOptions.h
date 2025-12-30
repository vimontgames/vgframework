#pragma once
#include "core/Options/Options.h"
#include "physics/Physics_Consts.h"

namespace vg::physics
{
    class IPhysicsOptions : public core::Options
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IPhysicsOptions, core::Options);

        virtual bool                        IsAnyBodyVisible            () const = 0;
        virtual bool                        IsBodyVisible               (ShapeType _shape, MotionType _motionType) const = 0;
        virtual bool                        IsConstraintVisible         (ConstraintType _constraint) const = 0;

        virtual bool                        IsMergeStaticBodiesEnabled  () const = 0;

        virtual physics::Category           GetPhysicsCategory          (const core::string & _name) const = 0;
        virtual core::vector<core::string>  GetPhysicsCategoryNames     () const = 0;
    };
}