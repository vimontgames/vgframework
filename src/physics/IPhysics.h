#pragma once

#include "core/IPlugin.h"
#include "Physics_Consts.h"

namespace vg
{
    namespace core
    {
        class IWorld;
        class IPhysicsWorld;
    }

    namespace physics
    {
        class IShapeDesc;
        class IShape;

        class IBodyDesc;
        class IBody;

        class ICharacterDesc;
        class ICharacter;

        class IPhysicsOptions;

	    class IPhysics : public core::IPlugin
	    {
	    public:
            IPhysics(const core::string & _name, core::IObject * _parent) :
                core::IPlugin(_name, _parent)
            {

            }

            virtual void                    Init                (const PhysicsCreationParams & _params, core::Singletons & _singletons) = 0;
            virtual void                    Deinit              () = 0;

            virtual void                    RunOneFrame         (float _dt) = 0;

            virtual IPhysicsOptions *       GetOptions          () const = 0;
            virtual void                    SetGravity          (const core::float3 _gravity) = 0;

            virtual core::IPhysicsWorld *   CreatePhysicsWorld  (const core::IWorld * _world) = 0;
            virtual IShape *                CreateShape         (const IShapeDesc * _shapeDesc) = 0;
            virtual IBody *                 CreateBody          (core::IPhysicsWorld * _physicsWorld, const IBodyDesc * _bodyDesc, IShape * _shape, const core::float4x4 & _matrix, const core::string & _name, core::IObject * _parent) = 0;
            virtual IBody *                 CreateBody          (core::IPhysicsWorld * _physicsWorld, const IBodyDesc * _bodyDesc, const core::vector<ShapeInfo> & _shapes, const core::float4x4 & _matrix, const core::string & _name, core::IObject * _parent) = 0;
            virtual ICharacter *            CreateCharacter     (core::IPhysicsWorld * _physicsWorld, const ICharacterDesc * _characterDesc, IShape * _shape, const core::float4x4 & _matrix, const core::string & _name, core::IObject * _parent) = 0;
            virtual ICharacter *            CreateCharacter     (core::IPhysicsWorld * _physicsWorld, const ICharacterDesc * _characterDesc, const core::vector<IShape*> & _shapes, const core::float4x4 & _matrix, const core::string & _name, core::IObject * _parent) = 0;
            virtual void                    RemoveBody          (core::IPhysicsWorld * _physicsWorld, IBody * _body) = 0;
	    };
    }
}