#pragma once

#include "engine/IVehicleComponent.h"
#include "physics/Physics_Consts.h"
#include "physics/IVehicleConstraint.h"

namespace vg::engine
{
    class VehicleComponent final : public IVehicleComponent
    {
    public:
        VG_CLASS_DECL(VehicleComponent, IVehicleComponent);

        VehicleComponent(const core::string & _name, IObject * _parent);
        ~VehicleComponent();

        void                OnPropertyChanged           (IObject * _object, const core::IProperty & _prop, bool _notifyParent);

        void                OnLoad                      ();
        void                OnPlay                      (); 
        void                OnStop                      ();

        void                FixedUpdate                 (const Context & _context) final override;
        void                Update                      (const Context & _context) final override;

    private:
        physics::IPhysics * getPhysics                  ();

        bool                createVehicleConstraintDesc ();
        bool                createVehicleConstraint     ();

    private:
        physics::DriveState               m_driveState;
        physics::IVehicleConstraintDesc * m_vehicleConstraintDesc = nullptr;
        physics::IVehicleConstraint *     m_vehicleConstraint = nullptr;
    };
}