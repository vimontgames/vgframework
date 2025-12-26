#pragma once

#include "core/Object/Object.h"
#include "Physics_Consts.h"

namespace vg::physics
{
    class IVehicleConstraintDesc : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IVehicleConstraintDesc, core::Object)

    };

    struct DriveState
    {
        float m_forward   = 0.0f;
        float m_right     = 0.0f;
        float m_brake     = 0.0f; 
        float m_handBrake = 0.0f;
    };

    class IVehicleConstraint: public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IVehicleConstraint, core::Object)

        virtual void            FixedUpdate     (DriveState _driveState) = 0;
        virtual void            Update          (DriveState _driveState) = 0;
        
        virtual core::float4x4  GetWheelMatrix  (core::uint _index) const = 0;
        virtual void            Reset           () = 0;
    };
}