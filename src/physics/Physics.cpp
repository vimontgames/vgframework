#include "physics/Precomp.h"
#include "physics.h"

#if !VG_ENABLE_INLINE
#include "Physics.inl"
#endif

using namespace vg::core;
using namespace vg::physics;

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
    Physics::Physics(const core::string & _name, core::IObject * _parent) :
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
        core::IFactory * factory = Kernel::getFactory();

        // Register classes to auto-register the "Physics" module
        AutoRegisterClassInfo::registerClasses(*factory);

        if (core::IClassDesc * desc = factory->registerPlugin(Physics, "Physics"))
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
	void Physics::Init(const PhysicsCreationParams & _params, Singletons & _singletons)
	{
        Kernel::setSingletons(_singletons);
        RegisterClasses();
	}

	//--------------------------------------------------------------------------------------
	void Physics::Deinit()
	{
        UnregisterClasses();
	}

	//--------------------------------------------------------------------------------------
	void Physics::RunOneFrame()
	{
        VG_PROFILE_CPU("Physics");

        
	}
}