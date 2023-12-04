#include "physics/Precomp.h"
#include "physics.h"
#include "JobSystem/JobSystemAdapter.h"
#include "Shape/Shape.h"
#include "physics/Helper.h"
#include "physics/Body/Body.h"
#include "core/Math/Math.h"

// Jolt includes
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Physics/Body/BodyManager.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include "engine/IEngine.h"
#include "renderer/IRenderer.h"
#include "renderer/IDebugDraw.h"

#if !VG_ENABLE_INLINE
#include "Physics.inl"
#endif

using namespace vg::core;
using namespace vg::physics;
using namespace JPH;

#define VG_PHYSICS_VERSION_MAJOR 0
#define VG_PHYSICS_VERSION_MINOR 0

// Avoid stripping code for classes from static lib
//static Shape universe("", nullptr);

//--------------------------------------------------------------------------------------
IPhysics * CreateNew()
{
	return new Physics("Physics", nullptr);
}

//--------------------------------------------------------------------------------------
namespace vg::physics
{
	//--------------------------------------------------------------------------------------
	IPlugin::Version Physics::GetVersion() const
	{
		return { VG_PHYSICS_VERSION_MAJOR, VG_PHYSICS_VERSION_MINOR };
	}

	//--------------------------------------------------------------------------------------
    Physics::Physics(const string & _name, IObject * _parent) :
        super(_name, _parent)
	{
        
	}

	//--------------------------------------------------------------------------------------
    Physics::~Physics()
	{

	}

    //--------------------------------------------------------------------------------------
    bool Physics::RegisterClasses()
    {
        IFactory * factory = Kernel::getFactory();

        // Register classes to auto-register the "Physics" module
        AutoRegisterClassInfo::registerClasses(*factory);

        if (IClassDesc * desc = factory->registerPlugin(Physics, "Physics"))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Physics::UnregisterClasses()
    {
        IFactory * factory = Kernel::getFactory();
        return AutoRegisterClassInfo::unregisterClasses(*factory);
    }

    //--------------------------------------------------------------------------------------
    bool Physics::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    static void TraceImpl(const char * inFMT, ...)
    {
        va_list list;
        va_start(list, inFMT);
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), inFMT, list);
        va_end(list);

        VG_INFO("[Physics] %s", buffer);
    }

    //--------------------------------------------------------------------------------------
    #ifdef JPH_ENABLE_ASSERTS
    static bool AssertFailedImpl(const char * inExpression, const char * inMessage, const char * inFile, uint inLine)
    {
        bool skip = false;
        if (assertmsg(inExpression, "", inFile, inLine, skip, inMessage))
            return true;
        
        // Do not break
        return false;
    };
    #endif // JPH_ENABLE_ASSERTS

    // test shapes
    //JPH::BodyID sphere_id;
    //JPH::Body * floor = nullptr;

	//--------------------------------------------------------------------------------------
	void Physics::Init(const PhysicsCreationParams & _params, Singletons & _singletons)
	{
        // Copy init params
        m_physicsCreationParams = _params;

        // Set our singletons used by this DLL
        Kernel::setSingletons(_singletons);

        // Register our classes declared in this DLL
        RegisterClasses();

        // Register Jolt allocation hook
        RegisterDefaultAllocator();

        // Register Jolt Trace/Assert callbacks
        Trace = TraceImpl;
        JPH_IF_ENABLE_ASSERTS(AssertFailed = AssertFailedImpl;)

        // Create a Jolt factory
        Factory::sInstance = new Factory();

        // Register all Jolt physics types
        RegisterTypes();
        
        // We need a temp allocator for temporary allocations during the physics update. We're
        // pre-allocating 10 MB to avoid having to do allocations during the physics update.
        // B.t.w. 10 MB is way too much for this example but it is a typical value you can use.
        // If you don't want to pre-allocate you can also use TempAllocatorMalloc to fall back to
        // malloc / free.
        //TempAllocatorImpl temp_allocator(10 * 1024 * 1024);

        // We need a job system that will execute physics jobs on multiple threads. Typically
        // you would implement the JobSystem interface yourself and let Jolt Physics run on top
        // of your own job scheduler. JobSystemThreadPool is an example implementation.
        m_jobSystem = new JobSystemAdapter(m_physicsCreationParams.maxPhysicsBarriers);

        // Now we can create the actual physics system
        m_physicsSystem = new JPH::PhysicsSystem();
        m_physicsSystem->Init(m_physicsCreationParams.maxBodies, m_physicsCreationParams.numBodyMutexes, m_physicsCreationParams.maxBodyPairs, m_physicsCreationParams.maxContactConstraints, m_broadPhaseLayer, m_broadPhaseFilter, m_objectFilter);
        m_physicsSystem->SetGravity(JPH::Vec3(0, 0, -9.81f));

        // The main way to interact with the bodies in the physics system is through the body interface. There is a locking and a non-locking
        // variant of this. We're going to use the locking version (even though we're not planning to access bodies from multiple threads)
        BodyInterface & body_interface = m_physicsSystem->GetBodyInterface();

        // test : add floor
        //{
        //    // Next we can create a rigid body to serve as the floor, we make a large box
        //    // Create the settings for the collision volume (the shape).
        //    // Note that for simple shapes (like boxes) you can also directly construct a BoxShape.
        //    JPH::BoxShapeSettings floor_shape_settings(Vec3(128.0f, 128.0f, 0.1f));
        //
        //    // Create the shape
        //    ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
        //    ShapeRefC floor_shape = floor_shape_result.Get(); // We don't expect an error here, but you can check floor_shape_result for HasError() / GetError()
        //
        //    // Create the settings for the body itself. Note that here you can also set other properties like the restitution / friction.
        //    BodyCreationSettings floor_settings(floor_shape, RVec3(0.0f, 0.0f, -0.1f), Quat::sIdentity(), getJoltMotionType(MotionType::Static), getJoltObjectLayer(ObjectLayer::NonMoving));
        //
        //    // Create the actual rigid body
        //    floor = body_interface.CreateBody(floor_settings); // Note that if we run out of bodies this can return nullptr
        //
        //    // Add it to the world
        //    body_interface.AddBody(floor->GetID(), EActivation::DontActivate);
        //}

        // Now create a dynamic body to bounce on the floor
        {
            //// Note that this uses the shorthand version of creating and adding a body to the world
            //BodyCreationSettings sphere_settings(new JPH::SphereShape(0.5f), RVec3(0.0f, 0.0f, 5.0f), Quat::sIdentity(), getJoltMotionType(MotionType::Dynamic), getJoltObjectLayer(ObjectLayer::Moving));
            //sphere_id = body_interface.CreateAndAddBody(sphere_settings, EActivation::Activate);
            //
            //// Now you can interact with the dynamic body, in this case we're going to give it a velocity.
            //// (note that if we had used CreateBody then we could have set the velocity straight on the body before adding it to the physics system)
            //body_interface.SetLinearVelocity(sphere_id, Vec3(0.0f, 0.0f, 0.0f));
        }

        // Optional step: Before starting the physics simulation you can optimize the broad phase. This improves collision detection performance (it's pointless here because we only have 2 bodies).
        // You should definitely not call this every frame or when e.g. streaming in a new level section as it is an expensive operation.
        // Instead insert all new objects in batches instead of 1 at a time to keep the broad phase efficient.
        //m_physicsSystem->OptimizeBroadPhase();
	}

	//--------------------------------------------------------------------------------------
	void Physics::Deinit()
	{
        // Unregisters all types with the factory and cleans up the default material
        UnregisterTypes();

        // Destroy test objects
        {
            BodyInterface & body_interface = m_physicsSystem->GetBodyInterface();

            // Remove the sphere from the physics system. Note that the sphere itself keeps all of its state and can be re-added at any time.
            //body_interface.RemoveBody(sphere_id);
            //
            //// Destroy the sphere. After this the sphere ID is no longer valid.
            //body_interface.DestroyBody(sphere_id);

            // Remove and destroy the floor
            //body_interface.RemoveBody(floor->GetID());
            //body_interface.DestroyBody(floor->GetID());
        }

        // Destroy the Jolt factory
        delete Factory::sInstance;
        Factory::sInstance = nullptr;

        VG_SAFE_DELETE(m_jobSystem);
        VG_SAFE_DELETE(m_physicsSystem);

        // Unregister our classes
        UnregisterClasses();
	}

    //--------------------------------------------------------------------------------------
    engine::IEngine * Physics::getEngine() const
    {
        return this->m_physicsCreationParams.engine;
    }

    //--------------------------------------------------------------------------------------
    renderer::IDebugDraw * Physics::getDebugDraw() const
    {
        return getEngine()->GetRenderer()->GetDebugDraw();
    }

	//--------------------------------------------------------------------------------------
	void Physics::RunOneFrame(double _dt)
	{
        VG_PROFILE_CPU("Physics");

        //BodyInterface & body_interface = m_physicsSystem->GetBodyInterface();        

        if (getEngine()->IsPlaying())
            m_physicsSystem->Update((float)(_dt*0.001f), 1, &m_tempAllocator, m_jobSystem);           

        // debug
        {
            //RVec3 position = body_interface.GetCenterOfMassPosition(sphere_id);
            //float3 center = float3(position.GetX(), position.GetY(), position.GetZ());
            //getDebugDraw()->AddWireframeBox(center - 0.1f, center + 0.1f, 0xFF0000FF);
            //if (getEngine()->IsPlaying())
            //    VG_INFO("[Physics] Position %.2f %.2f %.2f", (float)center.x, (float)center.y, (float)center.z);
        }
	}

    //--------------------------------------------------------------------------------------
    IShape * Physics::CreateShape(const IShapeDesc * _shapeDesc)
    {
        IShape * shape = nullptr;
        VG_ASSERT(_shapeDesc);
        if (nullptr != _shapeDesc)
        {
            const auto shapeType = _shapeDesc->GetShapeType();
            switch (shapeType)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(shapeType);
                    break;

                case ShapeType::Sphere:
                    shape = new SphereShape(*(const SphereShapeDesc*)_shapeDesc);
                    break;

                case ShapeType::Box:
                    shape = new BoxShape(*(const BoxShapeDesc *)_shapeDesc);
                    break;

            }
        }
        return shape;
    }

    //--------------------------------------------------------------------------------------
    IBody * Physics::CreateBody(const IBodyDesc * _bodyDesc, IShape * _shape, const core::float4x4 & _world)
    {
        const BodyType bodyType = _bodyDesc->GetBodyType();
        switch (bodyType)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(bodyType);
                return nullptr;

            case BodyType::Rigid:
                return new physics::RigidBody((RigidBodyDesc *)_bodyDesc, (Shape*)_shape, _world);
        }
    }
}