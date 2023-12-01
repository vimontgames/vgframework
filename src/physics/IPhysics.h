#pragma once

#include "core/IPlugin.h"
#include "Physics_Consts.h"

namespace vg
{
    namespace physics
    {
	    class IPhysics : public core::IPlugin
	    {
	    public:
            IPhysics(const core::string & _name, core::IObject * _parent) :
                core::IPlugin(_name, _parent)
            {

            }

            virtual void    Init            (const PhysicsCreationParams & _params, core::Singletons & _singletons) = 0;
            virtual void    Deinit          () = 0;

            virtual void    RunOneFrame     () = 0;
	    };
    }
}