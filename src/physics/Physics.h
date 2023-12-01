#pragma once

#include "physics/IPhysics.h"
#include "core/Singleton/Singleton.h"

namespace vg::physics
{
	class Physics : public IPhysics, public core::Singleton<Physics>
	{
	public:
        using super = IPhysics;

                                            Physics		        (const core::string & _name, core::IObject * _parent);
                                            ~Physics            ();

		IPlugin::Version				    GetVersion	        () const final override;
        const char *						getClassName		() const final override { return "Engine"; }

        bool                                RegisterClasses		() final override;
        bool                                UnregisterClasses	() final override;

        static bool                         registerProperties	(core::IClassDesc & _desc);

        void							    Init                (const PhysicsCreationParams & _params, core::Singletons & _singletons) final override;
        void							    Deinit              () final override;

        void							    RunOneFrame         () final override;
	};
}

#if VG_ENABLE_INLINE
#include "Physics.inl"
#endif