#pragma once

#include "physics/IPhysics.h"
#include "core/Singleton/Singleton.h"
#include "physics/BroadPhase/BroadPhaseLayer.h"
#include "physics/BroadPhase/BroadPhaseFilter.h"
#include "physics/BroadPhase/ObjectFilter.h"
#include "Jolt/Physics/PhysicsSystem.h"

namespace vg::renderer
{
    class IDebugDraw;
}

namespace JPH
{
    class BodyID;
}

namespace vg::physics
{
    class JobSystemAdapter;

	class Physics final : public IPhysics, public core::Singleton<Physics>
	{
	public:
        using super = IPhysics;

                                            Physics		                (const core::string & _name, core::IObject * _parent);
                                            ~Physics                    ();

		IPlugin::Version				    GetVersion	                () const final override;
        const char *						getClassName		        () const final override { return "Physics"; }

        bool                                RegisterClasses		        () final override;
        bool                                UnregisterClasses	        () final override;

        static bool                         registerProperties	        (core::IClassDesc & _desc);

        void							    Init                        (const PhysicsCreationParams & _params, core::Singletons & _singletons) final override;
        void							    Deinit                      () final override;

        void                                RunOneFrame                 (double _dt) final override;

        IShape *                            CreateShape                 (const IShapeDesc * _shapeDesc) final override;
        IBody *                             CreateBody                  (const IBodyDesc * _bodyDesc, IShape * _shape, const core::float4x4 & _world) final override;

        engine::IEngine *                   getEngine                   () const;
        renderer::IDebugDraw *              getDebugDraw                () const;

        JPH::PhysicsSystem *                getPhysicsSystem            () const { return m_physicsSystem; }

    private:
        PhysicsCreationParams               m_physicsCreationParams;
        JobSystemAdapter *                  m_jobSystem = nullptr;
        BroadPhaseLayer                     m_broadPhaseLayer;
        BroadPhaseFilter                    m_broadPhaseFilter;
        ObjectFilter                        m_objectFilter;
        JPH::PhysicsSystem *                m_physicsSystem = nullptr;
        JPH::TempAllocatorMalloc            m_tempAllocator;
	};
}

#if VG_ENABLE_INLINE
#include "Physics.inl"
#endif