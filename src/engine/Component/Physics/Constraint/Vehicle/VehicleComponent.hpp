#include "VehicleComponent.h"
#include "renderer/IRenderer.h"
#include "renderer/IDebugDraw.h"
#include "engine/ISoundComponent.h"
#include "engine/Component/Renderer/Instance/Mesh/MeshComponent.h"
#include "engine/IParticleComponent.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS(VehicleSlot, "Vehicle Slot");
    VG_REGISTER_OBJECT_CLASS(VehicleSlotList, "Vehicle Slot List");
    VG_REGISTER_COMPONENT_CLASS(VehicleComponent, "Vehicle", "Physics", "Physics vehicle controller.", editor::style::icon::VehicleController, 1);

    //--------------------------------------------------------------------------------------
    // VehicleSlot
    //--------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------
    void * ResizeVehicleSlotVector(IObject * _parent, uint _offset, uint _count)
    {
        auto vec = (core::vector<VehicleSlot> *)(uint_ptr(_parent) + _offset);
        vec->clear();
        vec->resize(_count);
        return vec->data();
    }

    //--------------------------------------------------------------------------------------
    bool VehicleSlot::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        setPropertyFlag(VehicleSlot, m_name, PropertyFlags::Hidden, false);
        registerResizeVectorFunc(VehicleSlot, ResizeVehicleSlotVector);

        registerPropertyEnum(VehicleSlot, VehicleSlotType, m_slotType, "Type");

        registerProperty(VehicleSlot, m_location, "Location");
        setPropertyDescription(VehicleSlot, m_location, "Seat location in vehicle");

        registerProperty(VehicleSlot, m_exit, "Exit");
        setPropertyDescription(VehicleSlot, m_exit, "Location when exiting vehicle");

        registerPropertyEx(VehicleSlot, m_owner, "Owner", PropertyFlags::Transient);
        setPropertyDescription(VehicleSlot, m_owner, "Current owner of the seat");

        return true;
    }

    //--------------------------------------------------------------------------------------
    // VehicleSlotList
    //--------------------------------------------------------------------------------------
    bool VehicleSlotList::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        return true;
    }

    //--------------------------------------------------------------------------------------
    // VehicleComponent
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

        registerPropertyEx(VehicleComponent, m_speedInKmPerHour, "Velocity", PropertyFlags::Transient | PropertyFlags::ReadOnly);
        setPropertyDescription(VehicleComponent, m_speedInKmPerHour, "Current speed in km/h");

        registerPropertyEx(VehicleComponent, m_localVelocity, "Local Velocity", PropertyFlags::Transient | PropertyFlags::ReadOnly);

        registerPropertyEx(VehicleComponent, m_engineRPM, "RPM", PropertyFlags::Transient | PropertyFlags::ReadOnly);
        setPropertyDescription(VehicleComponent, m_engineRPM, "Current engine Rotation Per Minute");

        registerPropertyEx(VehicleComponent, m_currentGear, "Gear", PropertyFlags::Transient | PropertyFlags::ReadOnly);

        registerPropertyObject(VehicleComponent, m_slots, "Slots");

        registerPropertyGroupBegin(VehicleComponent, "Contraints");
        {
            registerPropertyEnum(VehicleComponent, physics::VehicleType, m_vehicleType, "Type");
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

        // Do not update when position changes
        if (!strcmp("m_local", _prop.GetName()))
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

            switch (m_vehicleType)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(m_vehicleType);
                    break;

                case physics::VehicleType::FourWheels:
                    m_vehicleConstraintDesc = (physics::IVehicleConstraintDesc *)factory->CreateObject("FourWheelsVehicleConstraintDesc", "", this);
                    break;
            }

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
        m_startPos = GetGameObject()->GetGlobalMatrix();
        m_startPos[3].z += 1;

        resetDriveState();

        if (!m_vehicleConstraint)
            createVehicleConstraint();

        auto & slots = m_slots.getObjects();
        for (uint i = 0; i < slots.size(); ++i)
        {
            VehicleSlot & slot = slots[i];
            slot.m_owner.clear();
        }

        super::OnPlay();
    }

    //--------------------------------------------------------------------------------------
    void VehicleComponent::OnStop()
    {
        Brake(0.0f);

        super::OnStop();

        resetDriveState();

        VG_SAFE_RELEASE(m_vehicleConstraint);
    }

    //--------------------------------------------------------------------------------------
    void VehicleComponent::resetDriveState()
    {
        m_driveState.m_forward = 0.0f;
        m_driveState.m_right = 0.0f;
        m_driveState.m_brake = 0.0f;
        m_driveState.m_handBrake = 0.0f;

        Brake(0.0f);
        Handbrake(0.0f);

        // Force stop engine
        m_engineOn = true;
        StopEngine();
    }

    //--------------------------------------------------------------------------------------
    void VehicleComponent::FixedUpdate(const Context & _context)
    {
        if (m_vehicleConstraint)
            m_vehicleConstraint->FixedUpdate(m_driveState);// SetDriverInput
    }

    //--------------------------------------------------------------------------------------
    void VehicleComponent::Update(const Context & _context)
    {
        if (m_vehicleConstraint)
        {
            m_vehicleConstraint->Update(m_driveState);
            IGameObject * go = GetGameObject();

            // Update local velocity
            if (IPhysicsBodyComponent * bodyComp = go->GetComponentT<IPhysicsBodyComponent>())
            {
                const float3 velocity = bodyComp->GetLinearVelocity();

                const float4x4 mat = go->GetGlobalMatrix();

                m_localVelocity.x = dot(velocity, mat[0].xyz);
                m_localVelocity.y = dot(velocity, mat[1].xyz);
                m_localVelocity.z = dot(velocity, mat[2].xyz);

                m_speedInKmPerHour = length(velocity) * 60.0f * 60.0f / 1000.0f; // m/s to km/h

                m_engineRPM = m_vehicleConstraint->GetEngineRPM();
                m_currentGear = m_vehicleConstraint->GetCurrentGear();

                if (IGameObject * smokeGO = GetGameObject()->GetChildGameObject("Smoke"))
                {
                    if (IParticleComponent * smokePart = smokeGO->GetComponentT<IParticleComponent>())
                    {
                        const float forwardSpeed = GetLocalVelocity().x;
                        smokePart->SetSpawnRate(0, max(1.0f, forwardSpeed) * 4.0f);
                    }
                }                    
            }

            if (ISoundComponent * sound = go->GetComponentT<ISoundComponent>())
            {
                // Engine
                {
                    float engineVolume = 0.5f + 0.5f * saturate(m_localVelocity.x / 4.0f);

                    auto soundIndex = sound->GetSoundIndex("Engine");
                    if (-1 != soundIndex)
                        sound->SetVolume(soundIndex, engineVolume);
                }

                // Drift
                {
                    float lateralSpeedAbs = abs(m_localVelocity.y);

                    if (lateralSpeedAbs > 2.0f && m_localVelocity.z >= 0.0f)
                    {
                        auto soundIndex = sound->GetSoundIndex("Brake");
                        if (-1 != soundIndex)
                        {
                            if (!sound->IsPlaying(soundIndex))
                                sound->Play(soundIndex);
                        }
                    }
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    bool VehicleComponent::EnterVehicle(core::IGameObject * _owner, core::uint & _slotIndex)
    {
        auto & slots = m_slots.getObjects();
        for (uint i = 0; i < slots.size(); ++i)
        {
            VehicleSlot & slot = slots[i];
            if (slot.m_owner.getObject() == nullptr)
            {
                slot.m_owner.set(_owner);
                _slotIndex = i;

                if (ISoundComponent * sound = GetGameObject()->GetComponentT<ISoundComponent>())
                {
                    auto soundIndex = sound->GetSoundIndex("Enter");
                    if (-1 != soundIndex)
                        sound->Play(soundIndex);

                    // Start engine when driver enters the car
                    if (slot.m_slotType == VehicleSlotType::Driver)
                        StartEngine();
                }

                return true;
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool VehicleComponent::ExitVehicle(core::IGameObject * _owner)
    {
        auto & slots = m_slots.getObjects();
        for (uint i = 0; i < slots.size(); ++i)
        {
            VehicleSlot & slot = slots[i];
            if (slot.m_owner.getObject() == _owner)
            {
                slot.m_owner.clear();

                if (ISoundComponent * sound = GetGameObject()->GetComponentT<ISoundComponent>())
                {
                    auto soundIndex = sound->GetSoundIndex("Enter");
                    if (-1 != soundIndex)
                        sound->Play(soundIndex);
                }

                // Stop engine when the driver exits the car
                if (slot.m_slotType == VehicleSlotType::Driver)
                    StopEngine();

                return true;
            }
        }

        return false;
    }


    //--------------------------------------------------------------------------------------
    bool VehicleComponent::StartEngine()
    {
        if (!m_engineOn)
        {
            // start engine sound
            if (ISoundComponent * sound = GetGameObject()->GetComponentT<ISoundComponent>())
            {
                auto soundIndex = sound->GetSoundIndex("Engine");
                if (-1 != soundIndex)
                    sound->Play(soundIndex);
            }

            // start smoke VFX
            if (IGameObject * smokeGO = GetGameObject()->GetChildGameObject("Smoke"))
            {
                if (IParticleComponent * smokePart = smokeGO->GetComponentT<IParticleComponent>())
                    smokePart->Play();
            }

            m_engineOn = true;
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool VehicleComponent::StopEngine()
    {
        if (m_engineOn)
        {
            // stop engine sound
            if (ISoundComponent * sound = GetGameObject()->GetComponentT<ISoundComponent>())
            {
                auto soundIndex = sound->GetSoundIndex("Engine");
                if (-1 != soundIndex)
                    sound->Stop(soundIndex);
            }

            // stop smoke VFX
            if (IGameObject * smokeGO = GetGameObject()->GetChildGameObject("Smoke"))
            {
                if (IParticleComponent * smokePart = smokeGO->GetComponentT<IParticleComponent>())
                    smokePart->Stop();
            }

            m_engineOn = false;
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    void VehicleComponent::Respawn(core::float3 _offset)
    {
        if (m_vehicleConstraint)
        {
            PhysicsBodyComponent * bodyComp = (PhysicsBodyComponent *)GetGameObject()->GetComponentByType("PhysicsBodyComponent");

            // reset body
            bodyComp->SetLinearVelocity(float3(0, 0, 0));
            bodyComp->SetAngularVelocity(float3(0, 0, 0));

            // reset vehicle
            m_vehicleConstraint->Reset();

            // teleport back to spawn position + offset
            float4x4 mat = m_startPos;
            mat[3].xyz += _offset;
            bodyComp->SetMatrix(mat);

            // reset controls
            resetDriveState();
        }
    }

    //--------------------------------------------------------------------------------------
    uint VehicleComponent::GetPassengerSlotCount() const
    {
        return (uint)m_slots.Size();
    }

    //--------------------------------------------------------------------------------------
    core::IGameObject * VehicleComponent::GetPassengerSlotOwner(core::uint _index) const
    {
        const auto & slots = m_slots.getObjects();
        if (_index < slots.size())
            return VG_SAFE_STATIC_CAST(IGameObject, slots[_index].m_owner.getObject());
        else
            return nullptr;
    }

    //--------------------------------------------------------------------------------------
    core::IGameObject * VehicleComponent::GetPassengerSlotSeatLocation(core::uint _index) const
    {
        const auto & slots = m_slots.getObjects();
        if (_index < slots.size())
            return VG_SAFE_STATIC_CAST(IGameObject, slots[_index].m_location.getObject());
        else
            return nullptr;
    }

    //--------------------------------------------------------------------------------------
    core::IGameObject * VehicleComponent::GetPassengerSlotExitLocation(core::uint _index) const
    {
        const auto & slots = m_slots.getObjects();
        if (_index < slots.size())
            return VG_SAFE_STATIC_CAST(IGameObject, slots[_index].m_exit.getObject());
        else
            return nullptr;
    }

    //--------------------------------------------------------------------------------------
    VehicleSlotType VehicleComponent::GetPassengerSlotType(core::uint _index) const
    {
        return m_slots.getObjects()[_index].m_slotType;
    }

    //--------------------------------------------------------------------------------------
    core::float3 VehicleComponent::GetLocalVelocity() const
    {
        return m_localVelocity;
    }

    //--------------------------------------------------------------------------------------
    void VehicleComponent::Accelerate(float _forward)
    {
        _forward = clamp(_forward, -1.0f, 1.0f);
        m_driveState.m_forward = _forward;
    }

    //--------------------------------------------------------------------------------------
    void VehicleComponent::Brake(float _brake)
    {
        _brake = clamp(_brake, 0.0f, 1.0f);

        if (m_driveState.m_brake == 0.0f && _brake != 0.0f)
        {
            if (GetLocalVelocity().x > 1.0f)
            {
                if (ISoundComponent * sound = GetGameObject()->GetComponentT<ISoundComponent>())
                {
                    auto soundIndex = sound->GetSoundIndex("Brake");
                    if (-1 != soundIndex)
                        sound->Play(soundIndex);
                }
            }
        }

        if (MeshComponent * meshComponent = GetGameObject()->GetComponentT<MeshComponent>())
        {
            // TODO: Create IMaterialResource interface, make sure returned material is instanciated, and add methods that manipulate material resource and set values to renderer material
            const auto matIndex = meshComponent->GetMaterialIndex("Lights");
            if (-1 != matIndex)
            {
                IMaterialResource * mat = meshComponent->GetMaterialResource(matIndex);
                const auto propIndex = mat->GetPropertyIndex("m_emissiveIntensity");
                if (-1 != propIndex)
                    mat->SetProperty(propIndex, _brake);
            }
        }

        m_driveState.m_brake = _brake;    
    }

    //--------------------------------------------------------------------------------------
    void VehicleComponent::Steer(float _leftRight)
    {
        _leftRight = clamp(_leftRight, -1.0f, 1.0f);
        m_driveState.m_right = _leftRight;
    }

    //--------------------------------------------------------------------------------------
    void VehicleComponent::Handbrake(float _handbrake)
    {
        m_driveState.m_handBrake = _handbrake;
    }
}