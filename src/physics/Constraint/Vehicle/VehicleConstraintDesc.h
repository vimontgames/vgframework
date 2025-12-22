#pragma once

#include "physics/IVehicleConstraint.h"
#include "core/Object/ObjectList.h"
#include "core/Object/ObjectHandle.h"

namespace vg::physics
{
    vg_enum_class(vg::physics, VehicleWheelAxle, core::u8,
        Front = 0,
        Rear = 1
    );

    vg_enum_class(vg::physics, VehicleWheelSide, core::u8,
        Left = 0,
        Right = 1
     ); 

    //--------------------------------------------------------------------------------------
    class VehicleWheelDesc : public core::Object
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(VehicleWheelDesc, core::Object);

    public:
        core::ObjectHandle m_object;
        VehicleWheelAxle m_axle;
        VehicleWheelSide m_side;
        float m_radius = 0.3f;
        float m_width = 0.1f;
    };

    //--------------------------------------------------------------------------------------
    class VehicleWheelDescList : public core::ObjectList<VehicleWheelDesc>
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(VehicleWheelDescList, ObjectList<VehicleWheelDesc>);
    };

    //--------------------------------------------------------------------------------------
    class VehicleConstraintDesc : public IVehicleConstraintDesc
    {
    public:
        VG_CLASS_DECL(VehicleConstraintDesc, IVehicleConstraintDesc);

        VehicleConstraintDesc(const vg::core::string & _name = "", vg::core::IObject * _parent = nullptr);
        ~VehicleConstraintDesc();

        const VehicleWheelDescList & getWheelDescList() const { return m_wheelDescList; }

        void OnLoad() final override;

    public:
        bool m_fourWheelDrive = false;
        float m_maxPitchRollAngleInDegrees = 30.0f;

        VehicleWheelDescList m_wheelDescList;
    };
}