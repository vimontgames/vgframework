#include "VehicleComponent.h"
#include "renderer/IRenderer.h"
#include "renderer/IDebugDraw.h"
#include "engine/ISoundComponent.h"

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
        registerPropertyEx(VehicleSlot, m_owner, "Owner", PropertyFlags::Transient);

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

        registerPropertyEx(VehicleComponent, m_localVelocity, "Local velocity", PropertyFlags::Transient | PropertyFlags::ReadOnly);

        registerPropertyObject(VehicleComponent, m_slots, "Slots");

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
        m_startPos = GetGameObject()->GetGlobalMatrix();
        m_startPos[3].z += 1;

        if (!m_vehicleConstraint)
            createVehicleConstraint();

        super::OnPlay();
    }

    //--------------------------------------------------------------------------------------
    void VehicleComponent::OnStop()
    {
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

            // Update local velocity
            if (IPhysicsBodyComponent * bodyComp = GetGameObject()->GetComponentT<IPhysicsBodyComponent>())
            {
                const float3 velocity = bodyComp->GetLinearVelocity();

                m_localVelocity.x = dot(velocity, GetGameObject()->GetGlobalMatrix()[0].xyz);
                m_localVelocity.y = dot(velocity, GetGameObject()->GetGlobalMatrix()[1].xyz);
                m_localVelocity.z = dot(velocity, GetGameObject()->GetGlobalMatrix()[2].xyz);
            }

            if (ISoundComponent * sound = GetGameObject()->GetComponentT<ISoundComponent>())
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
    bool VehicleComponent::EnterVehicle(core::IGameObject * _owner, VehicleSlotType & _slotType)
    {
        auto & slots = m_slots.getObjects();
        for (uint i = 0; i < slots.size(); ++i)
        {
            VehicleSlot & slot = slots[i];
            if (slot.m_owner.getObject() == nullptr)
            {
                slot.m_owner.set(_owner);

                if (ISoundComponent * sound = GetGameObject()->GetComponentT<ISoundComponent>())
                {
                    auto soundIndex = sound->GetSoundIndex("Enter");
                    if (-1 != soundIndex)
                        sound->Play(soundIndex);

                    //engine
                    {
                        auto soundIndex = sound->GetSoundIndex("Engine");
                        if (-1 != soundIndex)
                            sound->Play(soundIndex);
                    }
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

                    //engine
                    {
                        auto soundIndex = sound->GetSoundIndex("Engine");
                        if (-1 != soundIndex)
                            sound->Stop(soundIndex);
                    }
                }

                return true;
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    void VehicleComponent::Respawn()
    {
        if (m_vehicleConstraint)
        {
            PhysicsBodyComponent * bodyComp = (PhysicsBodyComponent *)GetGameObject()->GetComponentByType("PhysicsBodyComponent");

            // reset body
            bodyComp->SetLinearVelocity(float3(0, 0, 0));
            bodyComp->SetAngularVelocity(float3(0, 0, 0));

            // reset vehicle
            m_vehicleConstraint->Reset();

            // teleport back to spawn position
            bodyComp->SetMatrix(m_startPos);

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
    core::IGameObject * VehicleComponent::GetPassenger(core::uint _index) const
    {
        return VG_SAFE_STATIC_CAST(IGameObject, m_slots.getObjects()[_index].m_owner.getObject());
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

        m_driveState.m_brake = _brake;    
    }

    //--------------------------------------------------------------------------------------
    void VehicleComponent::Steer(float _leftRight)
    {
        _leftRight = clamp(_leftRight, -1.0f, 1.0f);
        m_driveState.m_right = _leftRight;
    }
}