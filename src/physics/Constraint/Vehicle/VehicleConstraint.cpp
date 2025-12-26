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

    static float sMaxSteeringAngle = degreesToRadians(45.0f);
    static bool	 sAntiRollbar = true;
    static bool	 sLimitedSlipDifferentials = true;
    static float sMaxEngineTorque = 750.0f;
    static float sClutchStrength = 10.0f;
    static float sFrontCasterAngle = 0.0f;
    static float sFrontCamber = 0.0f;
    static float sFrontToe = 0.0f;
    static float sFrontSuspensionForwardAngle = 0.0f;
    static float sFrontSuspensionSidewaysAngle = 0.0f;
    static float sFrontSuspensionMinLength = 0.3f;
    static float sFrontSuspensionMaxLength = 0.6f;
    static float sFrontSuspensionFrequency = 1.5f;
    static float sFrontSuspensionDamping = 0.5f;
    static float sRearSuspensionForwardAngle = 0.0f;
    static float sRearSuspensionSidewaysAngle = 0.0f;
    static float sRearCasterAngle = 0.0f;
    static float sRearCamber = 0.0f;
    static float sRearToe = 0.0f;
    static float sRearSuspensionMinLength = 0.3f;
    static float sRearSuspensionMaxLength = 0.6f;
    static float sRearSuspensionFrequency = 1.5f;
    static float sRearSuspensionDamping = 0.5f;

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

        // Create vehicle constraint
        JPH::VehicleConstraintSettings vehicle;
        vehicle.mMaxPitchRollAngle = degreesToRadians(_vehicleConstraintDesc->m_maxPitchRollAngleInDegrees);
        vehicle.mUp = getJoltVec3(float3(0, 0, 1));

        // Suspension 
        float3 frontSuspensionDir = normalize(float3(tan(sFrontSuspensionSidewaysAngle), tan(sFrontSuspensionForwardAngle), -1.0f));
        float3 rearSuspensionDir = normalize(float3(tan(sRearSuspensionSidewaysAngle), tan(sRearSuspensionForwardAngle), -1.0f));

        // Wheel up
        quaternion frontCamberRot = quaternion::rotation_axis(float3(1, 0, 0), sFrontCamber); // Tilt X
        quaternion rearCamberRot = quaternion::rotation_axis(float3(1, 0, 0), sRearCamber);

        float3 frontWheelUp = mul(float3(0, 0, 1), frontCamberRot); // Z-up
        float3 rearWwheelUp = mul(float3(0, 0, 1), rearCamberRot);
        
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

        // hack (TODO: pass Vehicle GameObject instead of Body? or body global matrix so that we can get wheels relative to body?)
        float4x4 bodyMat = _body->GetMatrix();

        // Get body center of mass
        auto & bodyLockInterface = m_joltPhysicsSystem->GetBodyLockInterface();

        auto & wheelDescs = _vehicleConstraintDesc->getWheelDescList().getObjects();

        for (uint i = 0; i < wheelDescs.size(); ++i)
        {
            const VehicleWheelDesc & wheelDesc = wheelDescs[i];

            #pragma push_macro("new")
            #undef new
            JPH::WheelSettingsWV * jphWheel = new JPH::WheelSettingsWV();
            #pragma pop_macro("new")

            IObject * obj = wheelDesc.m_object.getObject();
            if (IGameObject * go = VG_SAFE_STATIC_CAST(IGameObject, obj))
            {
                float4x4 wheelWorldMat = go->GetGlobalMatrix();
                float3 wheelWorldPos = wheelWorldMat[3].xyz;

                float4x4 wheelLocalMat = mul(wheelWorldMat, inverse(bodyMat));
                float3 wheelLocalPos = wheelLocalMat[3].xyz;

                if (i < 2)
                    wheelLocalPos.z += sFrontSuspensionMaxLength; 
                else
                    wheelLocalPos.z += sRearSuspensionMaxLength;

                jphWheel->mPosition = getJoltVec3(wheelLocalPos);
                jphWheel->mRadius = wheelDesc.m_radius;
                jphWheel->mWidth = wheelDesc.m_width;

                // Temp hardcoded wheels layout (time to figure out how it works)
                switch (wheelDesc.m_axle)
                {
                    // Front left
                    case VehicleWheelAxle::Front:
                    {
                        jphWheel->mSuspensionDirection = getJoltVec3(frontSuspensionDir);
                        jphWheel->mSteeringAxis = getJoltVec3(frontSteeringAxis);
                        jphWheel->mWheelUp = getJoltVec3(frontWheelUp);
                        jphWheel->mWheelForward = getJoltVec3(frontWheelForward);
                        jphWheel->mSuspensionMinLength = sFrontSuspensionMinLength;
                        jphWheel->mSuspensionMaxLength = sFrontSuspensionMaxLength;
                        jphWheel->mSuspensionSpring.mFrequency = sFrontSuspensionFrequency;
                        jphWheel->mSuspensionSpring.mDamping = sFrontSuspensionDamping;
                        jphWheel->mMaxSteerAngle = sMaxSteeringAngle;
                        jphWheel->mMaxBrakeTorque = 3000.0f;
                        jphWheel->mMaxHandBrakeTorque = 0.0f; 
                    }
                    break;

                    case VehicleWheelAxle::Rear:
                    {
                        jphWheel->mSuspensionDirection = getJoltVec3(rearSuspensionDir);
                        jphWheel->mSteeringAxis = getJoltVec3(rearSteeringAxis);
                        jphWheel->mWheelUp = getJoltVec3(rearWwheelUp);
                        jphWheel->mWheelForward = getJoltVec3(rearWheelForward);
                        jphWheel->mSuspensionMinLength = sRearSuspensionMinLength;
                        jphWheel->mSuspensionMaxLength = sRearSuspensionMaxLength;
                        jphWheel->mSuspensionSpring.mFrequency = sRearSuspensionFrequency;
                        jphWheel->mSuspensionSpring.mDamping = sRearSuspensionDamping;
                        jphWheel->mMaxSteerAngle = 0.0f;
                        jphWheel->mMaxBrakeTorque = 3000.0f;
                        jphWheel->mMaxHandBrakeTorque = 1; 
                    }
                    break;
                }

                vehicle.mWheels.push_back(jphWheel);
            }          
        }

        #pragma push_macro("new")
        #undef new
        auto * controller = new JPH::WheeledVehicleControllerSettings();
        #pragma pop_macro("new")
       
        vehicle.mController = controller;

        // Differential
        int leftFrontWheel = -1, rightFrontWheel = -1;
        int leftRearWheel = -1, rightRearWheel = -1;
        for (uint w = 0; w < wheelDescs.size(); ++w)
        {
            const VehicleWheelDesc & wheelDesc = wheelDescs[w];
            if (wheelDesc.m_axle == VehicleWheelAxle::Front)
            {
                if (wheelDesc.m_side == VehicleWheelSide::Left)
                    leftFrontWheel = w;
                else if (wheelDesc.m_side == VehicleWheelSide::Right)
                    rightFrontWheel = w;
            }
            else if (wheelDesc.m_axle == VehicleWheelAxle::Rear)
            {
                if (wheelDesc.m_side == VehicleWheelSide::Left)
                    leftRearWheel = w;
                else if (wheelDesc.m_side == VehicleWheelSide::Right)
                    rightRearWheel = w;
            }
        }

        if (_vehicleConstraintDesc->m_fourWheelDrive && -1 != leftRearWheel && -1 != rightRearWheel)
        {
            controller->mDifferentials.resize(2);
            controller->mDifferentials[0].mLeftWheel = leftFrontWheel;
            controller->mDifferentials[0].mRightWheel = rightFrontWheel;

            controller->mDifferentials[1].mLeftWheel = leftRearWheel;
            controller->mDifferentials[1].mRightWheel = rightRearWheel;

            // Split engine torque
            controller->mDifferentials[0].mEngineTorqueRatio = controller->mDifferentials[1].mEngineTorqueRatio = 0.5f;
        }
        else if (-1 != leftFrontWheel && -1 != rightFrontWheel)
        {
            controller->mDifferentials.resize(1);
            controller->mDifferentials[0].mLeftWheel = leftFrontWheel;
            controller->mDifferentials[0].mRightWheel = rightFrontWheel;
        }

        // Anti rollbars
        if (sAntiRollbar)
        {
            vehicle.mAntiRollBars.resize(0);

            for (uint a = 0; a < enumCount<VehicleWheelAxle>(); ++a)
            {
                int axleLeftWheel = -1, axleRightWheel = -1;
                for (uint w = 0; w < wheelDescs.size(); ++w)
                {
                    const VehicleWheelDesc & wheelDesc = wheelDescs[w];

                    if (wheelDesc.m_axle == (VehicleWheelAxle)a)
                    {
                        if (wheelDesc.m_side == VehicleWheelSide::Left)
                            axleLeftWheel = w;
                        else if (wheelDesc.m_side == VehicleWheelSide::Right)
                            axleRightWheel = w;
                    }

                    if (-1 != axleLeftWheel && -1 != axleRightWheel)
                        continue;
                }

                if (-1 != axleLeftWheel && -1 != axleRightWheel)
                {
                    JPH::VehicleAntiRollBar rollbar;
                    rollbar.mLeftWheel = axleLeftWheel;
                    rollbar.mRightWheel = axleRightWheel;
                    vehicle.mAntiRollBars.push_back(rollbar);
                }
            }
        }
                
        // Lock to get non-const Body* from BodyID
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
        m_joltPhysicsSystem->RemoveStepListener(m_joltVehicleConstraint);
        m_joltPhysicsSystem->RemoveConstraint(m_joltVehicleConstraint); 
    }  

    //--------------------------------------------------------------------------------------
    void VehicleConstraint::Reset()
    {
        if (m_vehicleConstraintDesc)
        {
            vector<VehicleWheelDesc> & wheelDescs = m_vehicleConstraintDesc->getWheelDescList().getObjects();
            
            for (uint i = 0; i < wheelDescs.size(); ++i)
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

                // Reset engine RPM
                vehicleController->GetEngine().SetCurrentRPM(0.0f);

                // Reset transmission (gear and clutch)
                vehicleController->GetTransmission().Set(0, 0.0f);

                // Reset differentials
                //for (JPH::Differentials & diff : vehicleController->GetDifferentials())
                //    diff.Reset();

                m_joltVehicleConstraint->ResetWarmStart();                
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void VehicleConstraint::FixedUpdate(DriveState _driveState)
    {
        JPH::BodyInterface & bodyInterface = m_vehicleBody->m_physicsWorld->getBodyInterface();
        bodyInterface.ActivateBody(m_vehicleBody->m_bodyID);

        // To get controller
        JPH::WheeledVehicleController * vehicleController = (JPH::WheeledVehicleController *)m_joltVehicleConstraint->GetController();

        // Update vehicle statistics
        vehicleController->GetEngine().mMaxTorque = sMaxEngineTorque;
        vehicleController->GetTransmission().mClutchStrength = sClutchStrength;

        // Set slip ratios to the same for everything
        float limited_slip_ratio = sLimitedSlipDifferentials ? 1.4f : FLT_MAX;
        vehicleController->SetDifferentialLimitedSlipRatio(limited_slip_ratio);
        for (JPH::VehicleDifferentialSettings & d : vehicleController->GetDifferentials())
            d.mLimitedSlipRatio = limited_slip_ratio;
        
        // Pass the input on to the constraint
        vehicleController->SetDriverInput(_driveState.m_forward, _driveState.m_right, _driveState.m_brake, _driveState.m_handBrake);

        m_joltVehicleConstraint->SetVehicleCollisionTester(m_joltVehicleCollisionTester);        
    }

    //--------------------------------------------------------------------------------------
    void VehicleConstraint::Update(DriveState _driveState)
    {
        // debug
        renderer::IDebugDraw * debugDraw = Physics::get()->getDebugDraw();

        if (m_joltVehicleConstraint)
        {
            vector<VehicleWheelDesc> & wheelDescs = m_vehicleConstraintDesc->getWheelDescList().getObjects();

            for (uint i = 0; i < wheelDescs.size(); ++i)
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
                    const VehicleWheelDesc & wheelDesc = wheelDescs[i];
                    if (wheelDesc.m_side == VehicleWheelSide::Left)
                    {
                        float3 leftWheelPos = wheelMatDebug[3].xyz;

                        for (uint j = 0; j < wheelDescs.size(); ++j)
                        {
                            if (i != j)
                            {
                                const VehicleWheelDesc & wheelDescComp = wheelDescs[j];
                                if (wheelDesc.m_axle == wheelDescComp.m_axle && wheelDescComp.m_side == VehicleWheelSide::Right)
                                {
                                    float3 rightWheelPos = fromJoltVec3(m_joltVehicleConstraint->GetWheelWorldTransform(j, JPH::Vec3::sAxisZ(), JPH::Vec3::sAxisY()).GetTranslation());
                                    debugDraw->AddLine(m_world, leftWheelPos, rightWheelPos, 0xFF0000FF);
                                    break;
                                }
                            }
                        }                       
                    }                    
                }

                // transform the wheel visual
                VehicleWheelDesc & wheelDesc = wheelDescs[i];
                IObject * obj = wheelDesc.m_object.getObject();
                if (IGameObject * wheelGameobject = VG_SAFE_STATIC_CAST(IGameObject, obj))
                {
                    float4x4 wheelMat = GetWheelMatrix(i);
                    wheelGameobject->SetGlobalMatrix(wheelMat);
                }
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
}                       

