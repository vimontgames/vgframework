#pragma once

#include "core/IPlugin.h"
#include "Physics_Consts.h"

namespace vg
{
    namespace physics
    {
        class IShapeDesc;
        class IShape;

        class IBodyDesc;
        class IBody;

	    class IPhysics : public core::IPlugin
	    {
	    public:
            IPhysics(const core::string & _name, core::IObject * _parent) :
                core::IPlugin(_name, _parent)
            {

            }

            virtual void        Init            (const PhysicsCreationParams & _params, core::Singletons & _singletons) = 0;
            virtual void        Deinit          () = 0;

            virtual void        RunOneFrame     (double _dt) = 0;

            virtual IShape *    CreateShape     (const IShapeDesc * _shapeDesc) = 0;
            virtual IBody *     CreateBody      (const IBodyDesc * _bodyDesc, IShape * _shape, const core::float4x4 & _world) = 0;
	    };
    }
}