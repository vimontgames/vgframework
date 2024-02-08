#include "physics/Precomp.h"
#include "physics.h"
#include "JobSystem/JobSystemAdapter.h"
#include "Shape/SphereShape.h"
#include "Shape/BoxShape.h"
#include "Shape/CapsuleShape.h"
#include "Helper.h"
#include "Body/Body.h"
#include "Character/Character.h"
#include "Options/PhysicsOptions.h"
#include "core/Math/Math.h"
#include "core/Timer/Timer.h"

#include "engine/IEngine.h"
#include "renderer/IRenderer.h"
#include "renderer/IDebugDraw.h"

#if !VG_ENABLE_INLINE
#include "Physics.inl"
#endif

#include "PhysicsWorld.hpp"

using namespace vg::core;
using namespace vg::physics;
using namespace JPH;

#define VG_PHYSICS_VERSION_MAJOR 0
#define VG_PHYSICS_VERSION_MINOR 0

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
        Timer::init();

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
        #pragma push_macro("new")
        #undef new
        Factory::sInstance = new Factory();
        #pragma pop_macro("new")        

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
        #pragma push_macro("new")
        #undef new
        m_jobSystem = new JobSystemAdapter(m_physicsCreationParams.maxPhysicsBarriers);
        #pragma pop_macro("new")  

        PhysicsOptions * physicsOptions = new PhysicsOptions("PhysicsOptions", this);
	}

	//--------------------------------------------------------------------------------------
	void Physics::Deinit()
	{
        // Unregisters all types with the factory and cleans up the default material
        UnregisterTypes();

        // Destroy test objects
        {
            //BodyInterface & body_interface = m_physicsSystem->GetBodyInterface();

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
        //VG_SAFE_DELETE(m_physicsSystem);

        PhysicsOptions * physicsOptions = PhysicsOptions::get();
        VG_SAFE_DELETE(physicsOptions);

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
	void Physics::RunOneFrame(float _dt)
	{
        VG_PROFILE_CPU("Physics");
        const auto * engine = getEngine();
       
        for (auto * physicsWorld : m_physicsWorlds)
        {
            auto * world = physicsWorld->getWorld();
            if (world->IsPlaying() && !world->IsPaused())
            {
                auto * physicsSystem = physicsWorld->getPhysicsSystem();
                physicsSystem->Update((float)(_dt), 1, &m_tempAllocator, m_jobSystem);
            }
        }
	}

    //--------------------------------------------------------------------------------------
    IPhysicsOptions * Physics::GetOptions() const
    {
        return PhysicsOptions::get();
    }

    //--------------------------------------------------------------------------------------
    void Physics::SetGravity(const core::float3 _gravity)
    {
        for (auto * physicsWorld : m_physicsWorlds)
        {
            auto * physicsSystem = physicsWorld->getPhysicsSystem();
            physicsSystem->SetGravity(JPH::Vec3(_gravity.x, _gravity.y, _gravity.z));
        }
    }

    //--------------------------------------------------------------------------------------
    core::IPhysicsWorld * Physics::CreatePhysicsWorld(const core::IWorld * _world)
    {
        PhysicsWorld * physWorld = new PhysicsWorld(_world);
        return physWorld;
    }

    //--------------------------------------------------------------------------------------
    void Physics::registerPhysicsWorld(PhysicsWorld * _physicsWorld)
    {
        m_physicsWorlds.push_back(_physicsWorld);
    }
    //--------------------------------------------------------------------------------------
    void Physics::unregisterPhysicsWorld(PhysicsWorld * _physicsWorld)
    {
        m_physicsWorlds.remove(_physicsWorld);
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

                case ShapeType::Capsule:
                    shape = new CapsuleShape(*(const CapsuleShapeDesc *)_shapeDesc);
                    break;
            }
        }
        return shape;
    }

    //--------------------------------------------------------------------------------------
    IBody * Physics::CreateBody(core::IPhysicsWorld * _physicsWorld, const IBodyDesc * _bodyDesc, IShape * _shape, const core::float4x4 & _matrix)
    {
        VG_ASSERT(_physicsWorld);

        const BodyType bodyType = _bodyDesc->GetBodyType();
        switch (bodyType)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(bodyType);
                return nullptr;

            case BodyType::Rigid:
                return new physics::RigidBody((PhysicsWorld*)_physicsWorld, (RigidBodyDesc *)_bodyDesc, (Shape*)_shape, _matrix);
        }
    }

    //--------------------------------------------------------------------------------------
    ICharacter * Physics::CreateCharacter(core::IPhysicsWorld * _world, const ICharacterDesc * _characterDesc, IShape * _shape, const core::float4x4 & _matrix)
    {
        VG_ASSERT(_world);

        const CharacterType characterType = _characterDesc->GetCharacterType();
        switch (characterType)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(characterType);
                return nullptr;

            case CharacterType::Rigid:
                return new physics::RigidCharacter((PhysicsWorld*)_world, (RigidCharacterDesc *)_characterDesc, (Shape *)_shape, _matrix);
        }
    }
}