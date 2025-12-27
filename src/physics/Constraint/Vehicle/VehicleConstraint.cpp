#include "physics/Precomp.h"
#include "core/Math/Math.h"
#include "core/IGameObject.h"
#include "core/IComponent.h"
#include "VehicleConstraint.h"
#include "physics/Physics.h"
#include "physics/Body/Body.h"
#include "physics/World/PhysicsWorld.h"
#include "physics/Body/BodyDesc.h"
#include "renderer/IDebugDraw.h"
#include "physics/Options/PhysicsOptions.h"

#pragma push_macro("new")
#undef new
#include "Jolt/Physics/Vehicle/WheeledVehicleController.h"
#pragma pop_macro("new")

#include "Jolt/Physics/Vehicle/VehicleConstraint.h"
#include "Jolt/Physics/Vehicle/VehicleCollisionTester.h"


#include "VehicleConstraintDesc.hpp"

using namespace vg::core;

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    bool VehicleConstraint::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    static bool	 sAntiRollbar = true;
    static bool	 sLimitedSlipDifferentials = true;
    static float sFrontCasterAngle = 0.0f;
    static float sFrontCamber = 0.0f;
    static float sFrontToe = 0.0f;
    static float sRearCasterAngle = 0.0f;
    static float sRearCamber = 0.0f;
    static float sRearToe = 0.0f;

    //--------------------------------------------------------------------------------------
    VehicleConstraint::VehicleConstraint(Body * _body, const VehicleConstraintDesc * _vehicleConstraintDesc, const core::string & _name, core::IObject * _parent) :
        super(_name, _parent),
        m_vehicleConstraintDesc((VehicleConstraintDesc*)_vehicleConstraintDesc),
        m_vehicleBody(_body)
    {
        #pragma push_macro("new")
        #undef new
        m_joltVehicleCollisionTester = new JPH::VehicleCollisionTesterRay(getJoltObjectLayer(_body->getBodyDesc()->GetLayer()), JPH::Vec3(0,0,1));
        #pragma pop_macro("new")

        // Cache body world
        m_world = _body->GetParentGameObject()->GetWorld();

        // Cache physics world
        m_joltPhysicsSystem = _body->m_physicsWorld->getPhysicsSystem();

        // hack 
        float4x4 bodyMat = _body->GetMatrix();

        // Create vehicle constraint
        JPH::VehicleConstraintSettings vehicle;
        vehicle.mUp = getJoltVec3(float3(0, 0, 1));
        vehicle.mForward = getJoltVec3(float3(1, 0, 0));

        const uint wheelCount = _vehicleConstraintDesc->GetWheelCount();

        switch (_vehicleConstraintDesc->GetVehicleType())
        {
            default:
            {
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_vehicleConstraintDesc->GetVehicleType());
            }
            break;

            case VehicleType::FourWheels:
            {
                FourWheelsVehicleConstraintDesc * desc = (FourWheelsVehicleConstraintDesc *)_vehicleConstraintDesc;

                // Suspension 
                float3 frontSuspensionDir = normalize(float3(tan(degreesToRadians(desc->m_front.m_suspensionSidewaysAngleInDegree)), tan(degreesToRadians(desc->m_front.m_suspensionForwardAngleInDegree)), -1.0f));
                float3 rearSuspensionDir = normalize(float3(tan(degreesToRadians(desc->m_rear.m_suspensionSidewaysAngleInDegree)), tan(degreesToRadians(desc->m_rear.m_suspensionForwardAngleInDegree)), -1.0f));

                // Wheel up
                quaternion frontCamberRot = quaternion::rotation_axis(float3(1, 0, 0), sFrontCamber); // Tilt X
                quaternion rearCamberRot = quaternion::rotation_axis(float3(1, 0, 0), sRearCamber);

                float3 frontWheelUp = mul(float3(0, 0, 1), frontCamberRot); // Z-up
                float3 rearWheelUp = mul(float3(0, 0, 1), rearCamberRot);

                // Steering axis 
                quaternion frontCasterRot = quaternion::rotation_axis(float3(1, 0, 0), sFrontCasterAngle);
                quaternion rearCasterRot = quaternion::rotation_axis(float3(1, 0, 0), sRearCasterAngle);

                float3 frontSteeringAxis = mul(float3(0, 0, 1), frontCasterRot);
                float3 rearSteeringAxis = mul(float3(0, 0, 1), rearCasterRot);

                // Wheel forward 
                quaternion frontToeRot = quaternion::rotation_axis(float3(0, 0, 1), sFrontToe);
                quaternion rearToeRot = quaternion::rotation_axis(float3(0, 0, 1), sRearToe);

                float3 frontWheelForward = mul(float3(1, 0, 0), frontToeRot);
                float3 rearWheelForward = mul(float3(1, 0, 0), rearToeRot);

                vehicle.mMaxPitchRollAngle = degreesToRadians(desc->m_maxPitchRollAngleInDegrees);

                for (uint i = 0; i < wheelCount; ++i)
                {
                    VehicleAxleDesc & axle = (i < 2) ? desc->m_front : desc->m_rear;
                    ObjectHandle & handle = (i & 1) ? axle.m_rightWheel : axle.m_leftWheel;

                    #pragma push_macro("new")
                    #undef new
                    JPH::WheelSettingsWV * jphWheel = new JPH::WheelSettingsWV();
                    #pragma pop_macro("new")

                    IObject * obj = handle.getObject();
                    if (IGameObject * go = VG_SAFE_STATIC_CAST(IGameObject, obj))
                    {
                        float4x4 wheelWorldMat = go->GetGlobalMatrix();
                        float3 wheelWorldPos = wheelWorldMat[3].xyz;

                        float4x4 wheelLocalMat = mul(wheelWorldMat, inverse(bodyMat));
                        float3 wheelLocalPos = wheelLocalMat[3].xyz;
                        wheelLocalPos.z += axle.m_suspensionMaxLength;

                        // TODO: store in Front/Rear settings
                        if (i < 2)
                        {
                            // Front
                            jphWheel->mSuspensionDirection = getJoltVec3(frontSuspensionDir);
                            jphWheel->mSteeringAxis = getJoltVec3(frontSteeringAxis);
                            jphWheel->mWheelUp = getJoltVec3(frontWheelUp);
                            jphWheel->mWheelForward = getJoltVec3(frontWheelForward);                       
                        }
                        else
                        {
                            // Rear
                            jphWheel->mSuspensionDirection = getJoltVec3(rearSuspensionDir);
                            jphWheel->mSteeringAxis = getJoltVec3(rearSteeringAxis);
                            jphWheel->mWheelUp = getJoltVec3(rearWheelUp);
                            jphWheel->mWheelForward = getJoltVec3(rearWheelForward);
                        }

                        jphWheel->mPosition = getJoltVec3(wheelLocalPos);
                        jphWheel->mRadius = axle.m_radius;
                        jphWheel->mWidth = axle.m_width;
                        jphWheel->mMaxSteerAngle = degreesToRadians(axle.m_maxSteerAngleInDegrees);
                        jphWheel->mMaxBrakeTorque = axle.m_maxBrakeTorque;
                        jphWheel->mMaxHandBrakeTorque = axle.m_maxHandBrakeTorque;
                        jphWheel->mSuspensionMinLength = axle.m_suspensionMinLength;
                        jphWheel->mSuspensionMaxLength = axle.m_suspensionMaxLength;
                        jphWheel->mSuspensionSpring.mFrequency = axle.m_suspensionFrequency;
                        jphWheel->mSuspensionSpring.mDamping = axle.m_suspensionDamping;

                        vehicle.mWheels.push_back(jphWheel);
                    }
                }
            }
            break;
        }

        #pragma push_macro("new")
        #undef new
        auto * controller = new JPH::WheeledVehicleControllerSettings();
        #pragma pop_macro("new")
       
        vehicle.mController = controller;

        switch (_vehicleConstraintDesc->GetVehicleType())
        {
            default:
            {
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_vehicleConstraintDesc->GetVehicleType());
            }
            break;

            case VehicleType::FourWheels:
            {
                FourWheelsVehicleConstraintDesc * desc = (FourWheelsVehicleConstraintDesc *)_vehicleConstraintDesc;

                if (desc->m_fourWheelDrive)
                {
                    controller->mDifferentials.resize(2);
                    controller->mDifferentials[0].mLeftWheel = 0;
                    controller->mDifferentials[0].mRightWheel = 1;

                    controller->mDifferentials[1].mLeftWheel = 2;
                    controller->mDifferentials[1].mRightWheel = 3;

                    // Split engine torque
                    controller->mDifferentials[0].mEngineTorqueRatio = controller->mDifferentials[1].mEngineTorqueRatio = 0.5f;
                }
                else
                {
                    controller->mDifferentials.resize(1);
                    controller->mDifferentials[0].mLeftWheel = 0;
                    controller->mDifferentials[0].mRightWheel = 1;
                }

                if (sAntiRollbar)
                {
                    vehicle.mAntiRollBars.resize(2);

                    JPH::VehicleAntiRollBar frontRollbar;
                    frontRollbar.mLeftWheel = 0;
                    frontRollbar.mRightWheel = 1;
                    vehicle.mAntiRollBars[0] = frontRollbar;

                    JPH::VehicleAntiRollBar rearRollbar;
                    rearRollbar.mLeftWheel = 2;
                    rearRollbar.mRightWheel = 3;
                    vehicle.mAntiRollBars[1] = rearRollbar;
                }
            }
            break;
        }
                
        // Lock to get non-const Body* from BodyID
        auto & bodyLockInterface = m_joltPhysicsSystem->GetBodyLockInterface();
        {
            JPH::BodyLockWrite lock(bodyLockInterface, _body->m_bodyID);
            VG_VERIFY(lock.Succeeded());
            {
                JPH::Body & body = lock.GetBody();
                #pragma push_macro("new")
                #undef new
                m_joltVehicleConstraint = new JPH::VehicleConstraint(body, vehicle);
                #pragma pop_macro("new")
            }
        }
        
        m_joltPhysicsSystem->AddConstraint(m_joltVehicleConstraint);
        m_joltPhysicsSystem->AddStepListener(m_joltVehicleConstraint);

        m_joltVehicleConstraint->SetVehicleCollisionTester(m_joltVehicleCollisionTester);
    }

    //--------------------------------------------------------------------------------------
    VehicleConstraint::~VehicleConstraint()
    {
        if (m_joltVehicleConstraint)
        {
            m_joltPhysicsSystem->RemoveStepListener(m_joltVehicleConstraint);
            m_joltPhysicsSystem->RemoveConstraint(m_joltVehicleConstraint);
        }
    }  

    //--------------------------------------------------------------------------------------
    void VehicleConstraint::Reset()
    {
        if (m_vehicleConstraintDesc)
        {
            const uint wheelCount = m_vehicleConstraintDesc->GetWheelCount();
            
            for (uint i = 0; i < wheelCount; ++i)
            {
                JPH::Wheel * wheel = m_joltVehicleConstraint->GetWheel(i);
            
                wheel->SetAngularVelocity(0.0f);
                wheel->SetSteerAngle(0.0f);
                wheel->SetRotationAngle(0.0f);
            }
            
            if (m_joltVehicleConstraint)
            {
                JPH::WheeledVehicleController * vehicleController = (JPH::WheeledVehicleController *)m_joltVehicleConstraint->GetController();
                
                vehicleController->SetDriverInput(0.0f, 0.0f, 0.0f, 0.0f);
                vehicleController->GetEngine().SetCurrentRPM(0.0f);
                vehicleController->GetTransmission().Set(0, 0.0f);
            
                m_joltVehicleConstraint->ResetWarmStart();                
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void VehicleConstraint::FixedUpdate(DriveState _driveState)
    {
        if (m_joltVehicleConstraint)
        {
            JPH::BodyInterface & bodyInterface = m_vehicleBody->m_physicsWorld->getBodyInterface();
            bodyInterface.ActivateBody(m_vehicleBody->m_bodyID);

            // To get controller
            JPH::WheeledVehicleController * vehicleController = (JPH::WheeledVehicleController *)m_joltVehicleConstraint->GetController();

            // Probably does not need to change every frame ...
            switch (m_vehicleConstraintDesc->GetVehicleType())
            {
                default:
                {
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(m_vehicleConstraintDesc->GetVehicleType());
                }
                break;

                case VehicleType::FourWheels:
                {
                    FourWheelsVehicleConstraintDesc * desc = (FourWheelsVehicleConstraintDesc *)m_vehicleConstraintDesc;

                    auto & engine = vehicleController->GetEngine();
                    engine.mMaxTorque = desc->m_maxEngineTorque;
                    engine.mMinRPM = desc->m_minRPM;
                    engine.mMaxRPM = desc->m_maxRPM;
                    vehicleController->GetTransmission().mClutchStrength = desc->m_clutchStrength;
                }
                break;
            }

            // Set slip ratios to the same for everything
            float limited_slip_ratio = sLimitedSlipDifferentials ? 1.4f : FLT_MAX;
            vehicleController->SetDifferentialLimitedSlipRatio(limited_slip_ratio);
            for (JPH::VehicleDifferentialSettings & d : vehicleController->GetDifferentials())
                d.mLimitedSlipRatio = limited_slip_ratio;

            // Pass the input on to the constraint
            vehicleController->SetDriverInput(_driveState.m_forward, _driveState.m_right, _driveState.m_brake, _driveState.m_handBrake);

            m_joltVehicleConstraint->SetVehicleCollisionTester(m_joltVehicleCollisionTester);
        }
    }

    //--------------------------------------------------------------------------------------
    void VehicleConstraint::Update(DriveState _driveState)
    {
        // debug
        renderer::IDebugDraw * debugDraw = Physics::get()->getDebugDraw();

        if (m_joltVehicleConstraint)
        {
            switch (m_vehicleConstraintDesc->GetVehicleType())
            {
                default:
                {
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(m_vehicleConstraintDesc->GetVehicleType());
                }
                break;

                case VehicleType::FourWheels:
                {
                    FourWheelsVehicleConstraintDesc * desc = (FourWheelsVehicleConstraintDesc *)m_vehicleConstraintDesc;

                    const uint wheelCount = desc->GetWheelCount();

                    for (uint i = 0; i < wheelCount; ++i)
                    {
                        const JPH::Wheel * wheel = m_joltVehicleConstraint->GetWheel(i);
                        const JPH::WheelSettings * wheelSettings = wheel->GetSettings();

                        const float radius = wheelSettings->mRadius;
                        const float width = wheelSettings->mWidth;

                        if (PhysicsOptions::get()->IsConstraintVisible(ConstraintType::Vehicle))
                        {
                            // debug shapes
                            float4x4 wheelMatDebug = fromJoltMatrix(m_joltVehicleConstraint->GetWheelWorldTransform(i, JPH::Vec3::sAxisZ(), JPH::Vec3::sAxisY()));
                            debugDraw->AddWireframeCylinder(m_world, radius, width, 0xFF0000FF, wheelMatDebug);

                            // Draw axles
                            if (0 == (i & 1))
                            {
                                float3 leftWheelPos = wheelMatDebug[3].xyz;
                                float3 rightWheelPos = fromJoltVec3(m_joltVehicleConstraint->GetWheelWorldTransform(i+1, JPH::Vec3::sAxisZ(), JPH::Vec3::sAxisY()).GetTranslation());
                                debugDraw->AddLine(m_world, leftWheelPos, rightWheelPos, 0xFF0000FF);
                            }
                        }

                        // transform the wheel visual
                        VehicleAxleDesc & axle = (i < 2) ? desc->m_front : desc->m_rear;
                        ObjectHandle & handle = (i & 1) ? axle.m_rightWheel : axle.m_leftWheel;

                        IObject * obj = handle.getObject();
                        if (IGameObject * wheelGameobject = VG_SAFE_STATIC_CAST(IGameObject, obj))
                        {
                            float4x4 wheelMat = GetWheelMatrix(i);
                            wheelGameobject->SetGlobalMatrix(wheelMat);
                        }
                    }
                }
                break;
            }
        }
    }

    //--------------------------------------------------------------------------------------
    core::float4x4 VehicleConstraint::GetWheelMatrix(uint _index) const
    {
        if (m_joltVehicleConstraint)
        {
            float4x4 wheelMat = fromJoltMatrix(m_joltVehicleConstraint->GetWheelWorldTransform(_index, -JPH::Vec3::sAxisY(), JPH::Vec3::sAxisZ())); // The cylinder we draw is aligned with Y so we specify that as rotational axis

            if (_index & 1)
            {
                // right wheel
                wheelMat[1].xyz *= -1;
                wheelMat[2].xyz *= -1;
            }

            return (float4x4 &)wheelMat;
        }

        return float4x4::identity();
    }

    //--------------------------------------------------------------------------------------
    float VehicleConstraint::GetEngineRPM() const
    {
        if (m_joltVehicleConstraint)
        {
            JPH::WheeledVehicleController * vehicleController = (JPH::WheeledVehicleController *)m_joltVehicleConstraint->GetController();
            return vehicleController->GetEngine().GetCurrentRPM();
        }
        return 0.0f;
    }

    //--------------------------------------------------------------------------------------
    core::uint VehicleConstraint::GetCurrentGear() const
    {
        if (m_joltVehicleConstraint)
        {
            JPH::WheeledVehicleController * vehicleController = (JPH::WheeledVehicleController *)m_joltVehicleConstraint->GetController();
            return vehicleController->GetTransmission().GetCurrentGear();
        }
        return 0;
    }
}                       

