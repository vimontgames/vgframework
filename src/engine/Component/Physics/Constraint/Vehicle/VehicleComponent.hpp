#include "VehicleComponent.h"
#include "renderer/IRenderer.h"
#include "renderer/IDebugDraw.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(VehicleComponent, "Vehicle", "Physics", "Physics vehicle controller.", editor::style::icon::VehicleController, 1);

    //--------------------------------------------------------------------------------------
    bool VehicleComponent::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEx(VehicleComponent, m_driveState.m_forward, "Forward", PropertyFlags::Transient);
        setPropertyDescription(VehicleComponent, m_driveState.m_forward, "Value between -1 and 1 indicating desired driving direction and amount the gas pedal is pressed");
        setPropertyRange(VehicleComponent, m_driveState.m_forward, float2(-1, +1));

        registerPropertyEx(VehicleComponent, m_driveState.m_right, "Steering", PropertyFlags::Transient);
        setPropertyDescription(VehicleComponent, m_driveState.m_right, "Value between -1 and 1 indicating desired steering angle (-1= left, 0 = center, 1 = right)");
        setPropertyRange(VehicleComponent, m_driveState.m_right, float2(-1, +1));

        registerPropertyEx(VehicleComponent, m_driveState.m_brake, "Brake", PropertyFlags::Transient);
        setPropertyDescription(VehicleComponent, m_driveState.m_brake, "Value between 0 and 1 indicating how strong the brake pedal is pressed");
        setPropertyRange(VehicleComponent, m_driveState.m_brake, float2(0, +1));

        registerPropertyEx(VehicleComponent, m_driveState.m_handBrake, "Handbrake", PropertyFlags::Transient);
        setPropertyDescription(VehicleComponent, m_driveState.m_handBrake, "Value between 0 and 1 indicating how strong the hand brake is pulled");
        setPropertyRange(VehicleComponent, m_driveState.m_handBrake, float2(0, +1));

        registerPropertyGroupBegin(VehicleComponent, "Contraints");
        {
            registerPropertyObjectPtrEx(VehicleComponent, m_vehicleConstraintDesc, "Constraints", PropertyFlags::Flatten);
        }
        registerPropertyGroupEnd(VehicleComponent);

        return true;
    }

    //--------------------------------------------------------------------------------------
    VehicleComponent::VehicleComponent(const core::string & _name, IObject * _parent) :
        super(_name, _parent)
    {
        if (m_vehicleConstraintDesc == nullptr)
            createVehicleConstraintDesc();

        if (m_vehicleConstraint == nullptr)
            createVehicleConstraint();

        EnableUpdateFlags(UpdateFlags::FixedUpdate | UpdateFlags::Update);
    }

    //--------------------------------------------------------------------------------------
    VehicleComponent::~VehicleComponent()
    {
        VG_SAFE_RELEASE(m_vehicleConstraintDesc);
        VG_SAFE_RELEASE(m_vehicleConstraint);
    }

    //--------------------------------------------------------------------------------------
    void VehicleComponent::OnPropertyChanged(IObject * _object, const core::IProperty & _prop, bool _notifyParent)
    {
        // Do not update when steering changes
        if (asBool(PropertyFlags::Transient & _prop.GetFlags()))
            return;

        if (_object == this)
        {
            createVehicleConstraintDesc();
            createVehicleConstraint();
        }
        else if (_object == m_vehicleConstraintDesc)
        {
            createVehicleConstraint();
        }

        super::OnPropertyChanged(_object, _prop, _notifyParent);
    }

    //--------------------------------------------------------------------------------------
    physics::IPhysics * VehicleComponent::getPhysics()
    {
        return Engine::get()->GetPhysics();
    }

    //--------------------------------------------------------------------------------------
    bool VehicleComponent::createVehicleConstraintDesc()
    {
        if (nullptr == m_vehicleConstraintDesc)
        {
            IFactory * factory = Kernel::getFactory();

            m_vehicleConstraintDesc = (physics::IVehicleConstraintDesc *)factory->CreateObject("VehicleConstraintDesc", "", this);

            if (m_vehicleConstraintDesc)
            {
                m_vehicleConstraintDesc->SetParent(this);
                m_vehicleConstraintDesc->RegisterUID();
            }
        }

        return nullptr != m_vehicleConstraintDesc;
    }

    //--------------------------------------------------------------------------------------
    bool VehicleComponent::createVehicleConstraint()
    {
        VG_SAFE_RELEASE(m_vehicleConstraint);

        if (auto * world = GetGameObject()->GetWorld())
        {
            if (nullptr == world || false == world->IsPlaying())
                return false;

            // Find body component            
            if (PhysicsBodyComponent * bodyComp = GetGameObject()->GetComponentT<PhysicsBodyComponent>())
            {
                if (physics::IBody * body = bodyComp->getBody())
                {
                    VG_ASSERT(m_vehicleConstraintDesc);
                    if (m_vehicleConstraintDesc)
                        m_vehicleConstraint = getPhysics()->CreateVehicleConstraint(body, m_vehicleConstraintDesc, GetGameObject()->GetName() + "_VehicleConstraint", this);
                }
                else
                {
                    VG_ERROR("[Vehicle] VehicleComponent requires a PhysicsBodyComponent with a body");
                }
            }
            else
            {
                VG_ERROR("[Vehicle] VehicleComponent requires a PhysicsBodyComponent");
            }
        }
        return nullptr != m_vehicleConstraint;
    }

    //--------------------------------------------------------------------------------------
    void VehicleComponent::OnLoad()
    {
        createVehicleConstraint();
    }

    //--------------------------------------------------------------------------------------
    void VehicleComponent::OnPlay()
    {
        if (!m_vehicleConstraint)
            createVehicleConstraint();

        super::OnPlay();
    }

    //--------------------------------------------------------------------------------------
    void VehicleComponent::OnStop()
    {
        super::OnStop();

        VG_SAFE_RELEASE(m_vehicleConstraint);
    }

    //--------------------------------------------------------------------------------------
    void VehicleComponent::FixedUpdate(const Context & _context)
    {
        if (m_vehicleConstraint)
            m_vehicleConstraint->FixedUpdate(m_driveState);
    }

    //--------------------------------------------------------------------------------------
    void VehicleComponent::Update(const Context & _context)
    {
        if (m_vehicleConstraint)
            m_vehicleConstraint->Update(m_driveState);
    }
}