#include "physics/Precomp.h"
#include "Physics.h"
#include "JobSystem/JobSystemAdapter.h"
#include "Shape/SphereShape.h"
#include "Shape/BoxShape.h"
#include "Shape/CapsuleShape.h"
#include "Shape/CylinderShape.h"
#include "Shape/MeshShape.h"
#include "Body/Body.h"
#include "Character/Character.h"
#include "physics/Constraint/Vehicle/VehicleConstraint.h"
#include "physics/DebugRenderer/DebugRenderer.h"
#include "Options/PhysicsOptions.h"
#include "World/PhysicsWorld.h"
#include "core/Math/Math.h"
#include "core/Timer/Timer.h"

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
#define VG_PHYSICS_VERSION_MINOR 3

//--------------------------------------------------------------------------------------
IPhysics * CreateNew()
{
	return new Physics("Physics", nullptr);
}

//--------------------------------------------------------------------------------------
namespace vg::physics
{
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
        return AutoRegisterClassInfo::unregisterClasses();
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
	void Physics::Init(const PhysicsCreationParams & _params, const Callbacks & _callbacks, Singletons & _singletons)
	{
        // Set our singletons used by this DLL
        Kernel::setSingletons(_singletons);

        VG_PROFILE_CPU("Physics");

        Timer::init();

        // Copy init params
        m_physicsCreationParams = _params;

        // Store callbacks
        m_callbacks = _callbacks;

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

        #ifdef JPH_DEBUG_RENDERER
        #pragma push_macro("new")
        #undef new
        DebugRenderer * debugRenderer = new DebugRenderer();
        #pragma pop_macro("new")  
        JPH::DebugRenderer::sInstance = debugRenderer;
        #endif
	}

	//--------------------------------------------------------------------------------------
	void Physics::Deinit()
	{
        #ifdef JPH_DEBUG_RENDERER
        #pragma push_macro("delete")
        #undef delete
        DebugRenderer * debugRenderer = DebugRenderer::get();
        VG_SAFE_DELETE(debugRenderer);
        #pragma pop_macro("delete")  
        #endif

        // Unregisters all types with the factory and cleans up the default material
        UnregisterTypes();

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
	void Physics::Update(float _dt)
	{
        VG_PROFILE_CPU("Physics");
        const auto * engine = getEngine();

        PhysicsOptions::get()->Update();
       
        for (auto * physicsWorld : m_physicsWorlds)
            physicsWorld->Update(_dt, m_tempAllocator, m_jobSystem);

        for (auto * physicsWorld : m_physicsWorlds)
            physicsWorld->DrawDebug();

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
        vector_helper::remove(m_physicsWorlds, _physicsWorld);
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

                case ShapeType::Cylinder:
                    shape = new CylinderShape(*(const CylinderShapeDesc *)_shapeDesc);
                    break;

                case ShapeType::Mesh:
                    shape = new MeshShape(*(const MeshShapeDesc *)_shapeDesc);
                    break;
            }

            if (shape)
                VG_ASSERT(shape->GetShapeType() == shapeType);
        }
        return shape;
    }

    //--------------------------------------------------------------------------------------
    IBody * Physics::CreateBody(core::IPhysicsWorld * _physicsWorld, const IBodyDesc * _bodyDesc, const core::vector<ShapeInfo> & _shapes, const core::float4x4 & _matrix, const core::string & _name, core::IObject * _parent)
    {
        VG_ASSERT(_physicsWorld);

        const BodyType bodyType = _bodyDesc->GetBodyType();
        //VG_INFO("[Physics] Create %s body \"%s\"", asString(bodyType).c_str(), _name.c_str());

        switch (bodyType)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(bodyType);
                return nullptr;

            case BodyType::Rigid:
                return new physics::Body((PhysicsWorld *)_physicsWorld, (PhysicsBodyDesc *)_bodyDesc, _shapes, _matrix, _name, _parent);
        }
    }

    //--------------------------------------------------------------------------------------
    IBody * Physics::CreateBody(core::IPhysicsWorld * _physicsWorld, const IBodyDesc * _bodyDesc, IShape * _shape, const core::float4x4 & _matrix, const core::string & _name, core::IObject * _parent)
    {
        VG_ASSERT(_physicsWorld);

        const BodyType bodyType = _bodyDesc->GetBodyType();
        //VG_INFO("[Physics] Create %s body \"%s\"", asString(bodyType).c_str(), _name.c_str());

        switch (bodyType)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(bodyType);
                return nullptr;

            case BodyType::Rigid:
                return new physics::Body((PhysicsWorld*)_physicsWorld, (PhysicsBodyDesc *)_bodyDesc, (Shape*)_shape, _matrix, _name, _parent);
        }
    }

    //--------------------------------------------------------------------------------------
    ICharacter * Physics::CreateCharacter(core::IPhysicsWorld * _physicsWorld, const ICharacterDesc * _characterDesc, const core::vector<IShape *> & _shapes, const core::float4x4 & _matrix, const core::string & _name, core::IObject * _parent)
    {
        VG_ASSERT_NOT_IMPLEMENTED();
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    ICharacter * Physics::CreateCharacter(core::IPhysicsWorld * _world, const ICharacterDesc * _characterDesc, IShape * _shape, const core::float4x4 & _matrix, const core::string & _name, core::IObject * _parent)
    {
        VG_ASSERT(_world);

        const CharacterType characterType = _characterDesc->GetCharacterType();
        VG_INFO("[Physics] Create %s character \"%s\"", asString(characterType).c_str(), _name.c_str());

        switch (characterType)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(characterType);
                return nullptr;

            case CharacterType::Rigid:
                return new physics::RigidCharacter((PhysicsWorld*)_world, (RigidCharacterDesc *)_characterDesc, (Shape *)_shape, _matrix, _name, _parent);
        }
    }

    //--------------------------------------------------------------------------------------
    IVehicleConstraint * Physics::CreateVehicleConstraint(IBody * _body, IVehicleConstraintDesc * _vehicleConstraintDesc, const core::string & _name, core::IObject * _parent)
    {
        return new VehicleConstraint((Body*)_body, (VehicleConstraintDesc*) _vehicleConstraintDesc, _name, _parent);;
    }

    //--------------------------------------------------------------------------------------
    void Physics::SetDrawShapeCallback(DrawShapeCallback _drawShape)
    {
        m_drawShapeCallback = _drawShape;
    }
}